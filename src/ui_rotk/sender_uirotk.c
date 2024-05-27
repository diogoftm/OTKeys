#include "../../include/ui_rotk/sender_uirotk.h"
#include "../../include/etsi_004/models.h"
#include "../../include/etsi_004/ssl_socket_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <openssl/bio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <uuid/uuid.h>

struct MemoryStruct
{
    char *memory;
    size_t size;
};

// Callback function to handle received data from KMS
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        fprintf(stderr, "error: not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Send key id to the other player
void send_key_id(char *key_id, char *other_player_ip, unsigned int other_player_port)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "error: Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(other_player_port);

    if (inet_pton(AF_INET, other_player_ip, &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "error: Invalid address \n");
        return -1;
    }

    // Wait until the other player is available
    // Todo: add max tries
    while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        sleep(1);
    }

    send(sock, key_id, strlen(key_id), 0);
}

void sender_okd(OKDOT_SENDER *s)
{
    // Get key from the KMS

    if (s->mem == NULL || s->counter >= KEY_MEM_SIZE)
    {
        // ETSI 004 setup 
        char *cert_pem = SENDER_SAE_CRT, *key_pem = SENDER_SAE_KEY;
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

        // OPEN_CONNECT
        char *source = SENDER_SAE_ID;
        char *destination = s->other_player_sai_id;
        qkd_qos_t qos = {KEY_MEM_SIZE * KEY_LENGTH / 8, 32, 32, 0, 0, 0, 0, "application/json"};
        qkd_open_connect_request_t oc_request = {source, destination, qos, {}};
        uuid_parse("00000000-0000-0000-0000-000000000000", oc_request.key_stream_id);
        qkd_open_connect_response_t oc_response = qkd_open_connect_struct(oc_request, &cfg);

        if (oc_response.status != 0){
            fprintf(stderr, "Error: During OPEN_CONNECT got code %d from the KMS\n", oc_response.status);
            return;
        }

        // Send key stream id to the other player
        send_key_id(oc_response.key_stream_id, s->other_player_ip, s->other_player_port + s->my_num + 1);

        // GET_KEY
        qkd_key_info_t key_info = {qkd_key_type_oblivious, qkd_key_type_role_sender};
        qkd_get_key_request_t gk_request = {{}, 0, (qkd_metadata_t) {sizeof(qkd_key_info_t), (void*)&key_info}};
        uuid_copy(gk_request.key_stream_id, oc_response.key_stream_id);
        qkd_get_key_response_t gk_response = qkd_get_key_struct(gk_request, &cfg);

        if (gk_response.status != 0){
            fprintf(stderr, "Error: During GET_KEY got code %d from the KMS\n", gk_response.status);
            return;
        }

        // CLOSE
        qkd_close_request_t cl_request = {.key_stream_id={}};
        uuid_copy(cl_request.key_stream_id, oc_response.key_stream_id);
        qkd_close_response_t cl_response = qkd_close_struct(cl_request, &cfg);

        s->mem = gk_response.key_buffer.data;
    }

    // Save the key

    unsigned int *bitsArray = (unsigned int *)malloc(KEY_LENGTH * sizeof(unsigned int));
    if (bitsArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        bitsArray[i] = 0;
    }

    for (int i = 0; i < KEY_LENGTH/8 - 1; i++)
    {
        for (int d = 0; d < 8; d++)
        {
            bitsArray[i * 8 + d] = !!((s->mem[i] << d) & 0x80);
        }
    }

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        s->sender_OTkey[i] = bitsArray[i];
    }

    s->counter += 1;
    s->mem += KEY_LENGTH / 8;
}

void sender_output(OKDOT_SENDER *s, unsigned long long int *v0, unsigned long long int *v1, unsigned int *indexb,
                   unsigned int *indexb1, unsigned char (*output)[OUTPUT_LENGTH / 32])
{
    unsigned long int input32b[KEY_LENGTH / (2 * 32)] = {0};
    unsigned long int input32b1[KEY_LENGTH / (2 * 32)] = {0};

    /*converts the binary hash inputs into 32bit ints*/
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < KEY_LENGTH / (2 * 32); j++)
        {
            input32b[j] <<= 1;
            input32b1[j] <<= 1;

            input32b[j] += s->sender_OTkey[indexb[i + j * 32]] - '0';
            input32b1[j] += s->sender_OTkey[indexb1[i + j * 32]] - '0';
        }
    }

    /*hashes pairs of ints from the input32b and intput32b1 arrays into another 32bit value, which is then stored in the output array*/
    for (int i = 0; i < OUTPUT_LENGTH / 32; i++)
    {
        output[0][i] = (unsigned long int)((v0[0 + 3 * i] * input32b[0 + 2 * i] + v0[1 + 3 * i] * input32b[1 + 2 * i] + v0[2 + 3 * i]) >> 32);
        output[1][i] = (unsigned long int)((v1[0 + 3 * i] * input32b1[0 + 2 * i] + v1[1 + 3 * i] * input32b1[1 + 2 * i] + v1[2 + 3 * i]) >> 32);
    }
}