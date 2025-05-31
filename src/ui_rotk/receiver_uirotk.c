#include "../../include/ui_rotk/receiver_uirotk.h"
#include "../../include/ui_rotk/utils.h"

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

// Send key id to the other player
void send_key_id_(char *key_id, char *other_player_ip, unsigned int other_player_port)
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

// Get key id from the other player
char *receive_key_id_(char *my_ip, unsigned int my_port)
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
    char *receiver_strict_role = getenv("RECEIVER_STRICT_ROLE");
    if (receiver_strict_role == NULL)
        receiver_strict_role = "";

    if (r->mem == NULL || r->counter >= KEY_MEM_SIZE)
    {
        // Request key to the KMS

        struct MemoryStruct chunk;
        chunk.memory = malloc(1);
        chunk.size = 0;

        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            char url[256];
            if (strcmp(receiver_strict_role, "tx") == 0)
            {
                sprintf(url, "https://%s/api/v1/keys/%s/enc_keys?number=1&size=%d&key_type=1", KMS_URI, r->other_player_sai_id, KEY_MEM_SIZE * KEY_LENGTH);
            }
            else
            {
                // Get key id from the other player
                char *key_id = receive_key_id_(r->my_ip, r->my_port + r->other_player + 1);
                sprintf(url, "https://%s/api/v1/keys/%s/dec_keys?key_ID=%s", KMS_URI, r->other_player_sai_id, key_id);
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
            curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
            curl_easy_setopt(curl, CURLOPT_CAINFO, ROOT_CA);
            curl_easy_setopt(curl, CURLOPT_SSLCERT, RECEIVER_SAE_CRT);
            curl_easy_setopt(curl, CURLOPT_SSLKEY, RECEIVER_SAE_KEY);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            curl_easy_cleanup(curl);
        }

        json_t *root;
        json_error_t error;
        root = json_loads(chunk.memory, 0, &error);
        if (!root)
        {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return 1;
        }

        json_t *keys_array = json_object_get(root, "keys");
        if (!json_array_size(keys_array))
        {
            fprintf(stderr, "error: keys array is empty\n");
            json_decref(root);
            return 1;
        }

        json_t *key_obj = json_array_get(keys_array, 0);
        if (!key_obj)
        {
            fprintf(stderr, "error: failed to get key at index 0\n");
            json_decref(root);
            return 1;
        }

        json_t *key_value = json_object_get(key_obj, "key");
        if (!json_is_string(key_value))
        {
            fprintf(stderr, "error: key value is not a string\n");
            json_decref(root);
            return 1;
        }

        json_t *key_id_value = json_object_get(key_obj, "key_ID");
        if (!json_is_string(key_id_value))
        {
            fprintf(stderr, "error: key_ID value is not a string\n");
            json_decref(root);
            return 1;
        }

        const char *key_id = json_string_value(key_id_value);

        const char *key = json_string_value(key_value);
        size_t key_len = json_string_length(key_value);

        char *key_str = malloc(key_len + 1);
        if (key_str == NULL)
        {
            perror("QOT ERROR: Failed to allocate memory for key.\n");
            return 1;
        }
        memcpy(key_str, key, key_len);
        key_str[key_len] = '\0';

        // Decode key from base 64

        size_t out_len = b64_decoded_size(key_str) + 1;
        r->mem = malloc(out_len);
        b64_decode(key_str, (unsigned char *)r->mem, out_len);

        if (strcmp(receiver_strict_role, "tx") == 0)
            send_key_id(key_id, r->other_player_ip, r->other_player_port + r->my_num + 1);

        free(chunk.memory);

        r->counter = 0;
    }

    // Save the key

    unsigned int *bitsArray = (unsigned int *)malloc(KEY_LENGTH * sizeof(unsigned int));
    if (bitsArray == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        bitsArray[i] = 0;
    }

    for (int i = 0; i < KEY_LENGTH / 8 - 1; i++)
    {
        for (int d = 0; d < 8; d++)
        {
            if (strcmp(receiver_strict_role, "tx") != 0)
                bitsArray[i * 8 + d] = !!((r->mem[i] << d) & 0x80);
            else
            {
                if (d % 2 == 0)
                    bitsArray[i * 8 + d] = !!(((r->mem[i] << (d + 1)) ^ (r->mem[i] << (d))) & 0x80);
                else
                    bitsArray[i * 8 + d] = !!((r->mem[i] << (d - 1)) & 0x80);
            }
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

void receiver_output(OKDOT_RECEIVER *r, unsigned long long int *vb, unsigned int *output)
{

    unsigned int input32[KEY_LENGTH / (2 * 32)] = {0};

    for (int k = 0; k < KEY_LENGTH / 2; k++)
    {
        int index = r->indexlist[0][k];
        input32[k / 32] <<= 1;
        input32[k / 32] += r->receiver_OTkey[index] - '0';
    }

    for (int i = 0; i < KEY_LENGTH / 64; i++)
    {
        output[i] = (unsigned int)(((vb[i * 2] * input32[i] + vb[i * 2 + 1]) % PRIME) & 0xFFFFFFFF);
    }
}