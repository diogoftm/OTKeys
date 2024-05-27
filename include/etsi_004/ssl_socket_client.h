#ifndef ETSI_GS_QKD_004_SSL_SOCKET_CLIENT_H
#define ETSI_GS_QKD_004_SSL_SOCKET_CLIENT_H

#include "interfaces.h"

#ifdef __cplusplus__
extern "C" {
#endif

typedef struct qkd_client_context_data {
    char *host;
    uint16_t port;
    const char *cert_pem;
    char *key_pem;
    char *peer_ca_pem;
    int max_proto_version;
} qkd_client_context_data_t;

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_SSL_SOCKET_CLIENT_H
