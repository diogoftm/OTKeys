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

void receiver_okd(OKDOT_RECEIVER *r)
{
    // Get key from the KMS
    if (r->mem == NULL || r->counter >= KEY_MEM_SIZE)
    {
        // ETSI 004 setup
        char *cert_pem = RECEIVER_SAE_CRT, *key_pem = RECEIVER_SAE_KEY;
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
        qkd_key_info_t key_info = {qkd_key_type_oblivious, qkd_key_type_role_receiver};
        qkd_get_key_request_t gk_request = {{}, r->key_index, (qkd_metadata_t){sizeof(qkd_key_info_t), (void *)&key_info}};
        uuid_parse(r->ksid, gk_request.key_stream_id);
        qkd_get_key_response_t gk_response = qkd_get_key_struct(gk_request, &cfg);

        qkd_status_t res_status = gk_response.status;

        while (res_status != 0)
        {
            sleep(0.5);
            gk_response = qkd_get_key_struct(gk_request, &cfg);
            res_status = gk_response.status;
        }

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

    for (int i = 0; i < KEY_LENGTH / 8 - 1; i++)
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

    // generate I0 and I1 index lists from the receiver aux key
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

    // converts the binary hash inputs into 32bit ints
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < KEY_LENGTH / (2 * 32); j++)
        {
            input32[j] <<= 1;
            input32[j] += r->receiver_OTkey[r->indexlist[0][i + j * 32]] - '0';
        }
    }

    // hashes pairs of ints from the input32 array into another 32bit value, which is stored in the output array
    for (int i = 0; i < OUTPUT_LENGTH / 32; i++)
    {
        output[i] = (unsigned long int)((vb[0 + 3 * i] * input32[0 + 2 * i] + vb[1 + 3 * i] * input32[1 + 2 * i] + vb[2 + 3 * i]) >> 32);
    }
}
