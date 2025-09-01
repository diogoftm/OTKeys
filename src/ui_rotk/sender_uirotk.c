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
    char *sender_strict_role = getenv("SENDER_STRICT_ROLE");

    if (sender_strict_role == NULL)
        sender_strict_role = "";

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
                return;
            }
        }
        else
        {
            fprintf(stderr, "Error: Host not found in the input string.\n");
            return;
        }

        int max_proto_version;
        max_proto_version = 0;

        qkd_client_context_data_t cfg = {host, port, cert_pem, key_pem, peer_ca_pem, max_proto_version};

        // GET_KEY
        char role = qkd_key_type_role_sender;

        if (strcmp(sender_strict_role, "rx") == 0)
        {
            role = qkd_key_type_role_receiver;
        }

        qkd_key_info_t key_info = {qkd_key_type_oblivious, role};
        qkd_get_key_request_t gk_request = {{}, s->key_index, (qkd_metadata_t){sizeof(qkd_key_info_t), (void *)&key_info}};
        uuid_parse(s->ksid, gk_request.key_stream_id);
        qkd_get_key_response_t gk_response = qkd_get_key_struct(gk_request, &cfg);

        qkd_status_t res_status = gk_response.status;

        while (res_status != 0)
        {
            sleep(1);
            gk_response = qkd_get_key_struct(gk_request, &cfg);
            res_status = gk_response.status;
        }

        s->mem = gk_response.key_buffer.data;
        s->counter = 0;
    }

    // Save the key

    unsigned int *bitsArray = (unsigned int *)malloc(KEY_LENGTH * sizeof(unsigned int));
    if (bitsArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        bitsArray[i] = 0;
    }

    for (int i = 0; i < KEY_LENGTH / 8; i++)
    {
        for (int d = 0; d < 8; d++)
        {
            if (strcmp(sender_strict_role, "rx") != 0)
                bitsArray[i * 8 + d] = !!((s->mem[i] << d) & 0x80);
            else
            {
                if (d % 2 == 0)
                    bitsArray[i * 8 + d] = !!((s->mem[i] << (d + 1)) & 0x80);
                else
                    bitsArray[i * 8 + d] = !!(((s->mem[i] << d) ^ (s->mem[i] << (d - 1))) & 0x80);
            }
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
                   unsigned int *indexb1, unsigned int (*output)[KEY_LENGTH / 64])
{
    unsigned int input32b[KEY_LENGTH / (2 * 32)] = {0};
    unsigned int input32b1[KEY_LENGTH / (2 * 32)] = {0};

    for (int k = 0; k < KEY_LENGTH / 2; k++)
    {
        input32b[k / 32] <<= 1;
        input32b1[k / 32] <<= 1;

        input32b[k / 32] += s->sender_OTkey[indexb[k]] - '0';
        input32b1[k / 32] += s->sender_OTkey[indexb1[k]] - '0';
    }

    for (int i = 0; i < KEY_LENGTH / 64; i++)
    {
        output[0][i] = (unsigned int)(((v0[i * 2] * input32b[i] + v0[i * 2 + 1]) % PRIME) & 0xFFFFFFFF);
        output[1][i] = (unsigned int)(((v1[i * 2] * input32b1[i] + v1[i * 2 + 1]) % PRIME) & 0xFFFFFFFF);
    }
}