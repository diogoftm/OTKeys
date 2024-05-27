#include "../../include/ui_rotk/receiver_uirotk.h"
#include "../../include/etsi_004/models.h"
#include "../../include/etsi_004/ssl_socket_client.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <jansson.h>
#include <curl/curl.h>
#include <asm-generic/socket.h>

struct MemoryStruct
{
    char *memory;
    size_t size;
};

// Callback function to handle received data from KMS
size_t write_callback_(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        perror("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Get key id from the other player
char *receive_key_id(char *my_ip, unsigned int my_port)
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char *buffer = (char *)malloc(1024);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(my_ip);
    address.sin_port = htons(my_port);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             &addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024 - 1);
    if (valread < 0)
    {
        perror("read");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    buffer[valread] = '\0';

    close(new_socket);
    close(server_fd);

    return buffer;
}

void receiver_okd(OKDOT_RECEIVER *r)
{
    if (r->mem == NULL || r->counter >= KEY_MEM_SIZE)
    {
        // ETSI 004 setup 
        char *cert_pem = RECEIVER_SAE_CRT, *key_pem = RECEIVER_SAE_KEY;
        char *peer_ca_pem = NULL;

        char *host;
        int port;
        char input[] = KMS_URI;
        char *token = strtok(input, ":");
        if (token != NULL) {
            host = token;

            token = strtok(NULL, ":");
            if (token != NULL) {
                port = atoi(token);
            } else {
                fprintf(stderr, "Error: Port not found in the input string.\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Error: Host not found in the input string.\n");
            return 1;
        }

        int max_proto_version;
        max_proto_version = 0;

        qkd_client_context_data_t cfg = {host, port, cert_pem, key_pem, peer_ca_pem, max_proto_version};

        // Get key id from the other player
        char *key_stream_id = receive_key_id(r->my_ip, r->my_port + r->other_player + 1);

        // OPEN_CONNECT
        char* source = RECEIVER_SAE_ID;
        char* destination = r->other_player_sai_id;
        qkd_qos_t qos = {KEY_MEM_SIZE * KEY_LENGTH / 8, 32, 32, 0, 0, 0, 0, "application/json"};
        qkd_open_connect_request_t oc_request = {source, destination, qos, {}};
        memcpy(oc_request.key_stream_id, key_stream_id, 16);
        qkd_open_connect_response_t oc_response = qkd_open_connect_struct(oc_request, &cfg);

        // GET_KEY
        qkd_key_info_t key_info = {qkd_key_type_oblivious, qkd_key_type_role_receiver};
        qkd_get_key_request_t gk_request = {{}, 0, (qkd_metadata_t) {sizeof(qkd_key_info_t), (void*)&key_info}};
        uuid_copy(gk_request.key_stream_id, key_stream_id);
        qkd_get_key_response_t gk_response = qkd_get_key_struct(gk_request, &cfg);

        // CLOSE
        qkd_close_request_t cl_request = {.key_stream_id={}};
        uuid_copy(cl_request.key_stream_id, oc_response.key_stream_id);
        qkd_close_response_t cl_response = qkd_close_struct(cl_request, &cfg);

        r->mem = gk_response.key_buffer.data;
    }

    // Save the key

    unsigned int *bitsArray = (unsigned int *)malloc(KEY_LENGTH * sizeof(unsigned int));
    if (bitsArray == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 512; i++)
    {
        bitsArray[i] = 0;
    }

    for (int i = 0; i < KEY_LENGTH/8 - 1; i++)
    {
        for (int d = 0; d < 8; d++)
        {
            bitsArray[i * 8 + d] = !!((r->mem[i] << d) & 0x80);
        }
    }

    int i = 0;
    while (i < KEY_LENGTH / 2)
    {
        unsigned int aux_okey_uint = (unsigned int)bitsArray[2 * i];
        unsigned int okey_uint = (unsigned int)bitsArray[2 * i + 1];
        if (aux_okey_uint == 0) // If aux_key is zero
        {
            r->receiver_OTauxkey[2 * i] = 0;     // The first element is known
            r->receiver_OTauxkey[2 * i + 1] = 1; // The second element is unkown

            r->receiver_OTkey[2 * i] = okey_uint; // Saves the value known by the receiver
            r->receiver_OTkey[2 * i + 1] = 1;     // Saves 1: meaning it is unkown
        }
        else
        {
            r->receiver_OTauxkey[2 * i] = 1;     // The first element is unknown
            r->receiver_OTauxkey[2 * i + 1] = 0; // The second element is known

            r->receiver_OTkey[2 * i] = 1;             // Saves 1: meaning it is unkown
            r->receiver_OTkey[2 * i + 1] = okey_uint; // Saves the value known by the receiver
        }
        i++;
    }

    r->counter += 1;
    r->mem += KEY_LENGTH / 8;
}

void receiver_indexlist(OKDOT_RECEIVER *r)
{
    int j = 0;
    int k = 0;

    /*generate I0 and I1 index lists from the receiver aux key*/
    for (int i = 0; i < KEY_LENGTH; i++)
    {

        if (r->receiver_OTauxkey[i] == 0) // known bit
        {
            r->indexlist[0][j] = i;
            j++;
        }
        else if (r->receiver_OTauxkey[i] == 1) // unkown bit
        {
            r->indexlist[1][k] = i;
            k++;
        }
        else
            printf("OT ERROR: invalid key character found.\n");
    }
}

void receiver_output(OKDOT_RECEIVER *r, unsigned long long int *vb, unsigned char *output)
{
    unsigned long int input32[KEY_LENGTH / (2 * 32)] = {0};

    /*converts the binary hash inputs into 32bit ints*/
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < KEY_LENGTH / (2 * 32); j++)
        {
            input32[j] <<= 1;
            input32[j] += r->receiver_OTkey[r->indexlist[0][i + j * 32]] - '0';
        }
    }

    /*hashes pairs of ints from the input32 array into another 32bit value, which is stored in the output array*/
    for (int i = 0; i < OUTPUT_LENGTH / 32; i++)
    {
        output[i] = (unsigned long int)((vb[0 + 3 * i] * input32[0 + 2 * i] + vb[1 + 3 * i] * input32[1 + 2 * i] + vb[2 + 3 * i]) >> 32);
    }
}
