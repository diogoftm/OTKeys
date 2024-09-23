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
#include <unistd.h>

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
        if (token != NULL)
        {
            host = token;

            token = strtok(NULL, ":");
            if (token != NULL)
            {
                port = atoi(token);
            }
            else
            {
                fprintf(stderr, "Error: Port not found in the input string.\n");
                return 1;
            }
        }
        else
        {
            fprintf(stderr, "Error: Host not found in the input string.\n");
            return 1;
        }

        int max_proto_version;
        max_proto_version = 0;

        qkd_client_context_data_t cfg = {host, port, cert_pem, key_pem, peer_ca_pem, max_proto_version};

        // GET_KEY
        qkd_key_info_t key_info = {qkd_key_type_oblivious, qkd_key_type_role_sender};
        qkd_get_key_request_t gk_request = {{}, s->key_index, (qkd_metadata_t){sizeof(qkd_key_info_t), (void *)&key_info}};
        uuid_parse(s->ksid, gk_request.key_stream_id);
        qkd_get_key_response_t gk_response = qkd_get_key_struct(gk_request, &cfg);

        qkd_status_t res_status = gk_response.status;

        while (res_status != 0)
        {
            sleep(0.5);
            gk_response = qkd_get_key_struct(gk_request, &cfg);
            res_status = gk_response.status;
        }

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

    for (int i = 0; i < KEY_LENGTH / 8 - 1; i++)
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

    // converts the binary hash inputs into 32bit ints
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

    // hashes pairs of ints from the input32b and intput32b1 arrays into another 32bit value, which is then stored in the output array
    for (int i = 0; i < OUTPUT_LENGTH / 32; i++)
    {
        output[0][i] = (unsigned long int)((v0[0 + 3 * i] * input32b[0 + 2 * i] + v0[1 + 3 * i] * input32b[1 + 2 * i] + v0[2 + 3 * i]) >> 32);
        output[1][i] = (unsigned long int)((v1[0 + 3 * i] * input32b1[0 + 2 * i] + v1[1 + 3 * i] * input32b1[1 + 2 * i] + v1[2 + 3 * i]) >> 32);
    }
}