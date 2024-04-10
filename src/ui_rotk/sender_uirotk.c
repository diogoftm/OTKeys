#include "../../include/ui_rotk/sender_uirotk.h"
#include "../../include/ui_rotk/utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <jansson.h>
#include <openssl/bio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        char url[256];
        sprintf(url, "https://%s/api/v1/keys/%s/enc_keys?number=1&size=512&key_type=1", KMS_URI, s->other_player_sai_id);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
        curl_easy_setopt(curl, CURLOPT_CAINFO, ROOT_CA);
        curl_easy_setopt(curl, CURLOPT_SSLCERT, SENDER_SAE_CRT);
        curl_easy_setopt(curl, CURLOPT_SSLKEY, SENDER_SAE_KEY);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
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
        fprintf(stderr, "QOT ERROR: Failed to allocate memory for key.\n");
        return 1;
    }
    memcpy(key_str, key, key_len);
    key_str[key_len] = '\0';

    // Decode key from base 64

    size_t out_len = b64_decoded_size(key_str) + 1;
    char *out = malloc(out_len);
    b64_decode(key_str, (unsigned char *)out, out_len);
    out[out_len] = '\0';

    // Send key id to the other player

    send_key_id(key_id, s->other_player_ip, s->other_player_port + 1);

    // Save the key

    unsigned int *bitsArray = (unsigned int *)malloc(512 * sizeof(unsigned int));
    if (bitsArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 512; i++)
    {
        bitsArray[i] = 0;
    }

    for (int i = 0; i < out_len - 1; i++)
    {
        for (int d = 0; d < 8; d++)
        {
            bitsArray[i * 8 + d] = !!((out[i] << d) & 0x80);
        }
    }

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        s->sender_OTkey[i] = bitsArray[i];
    }
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