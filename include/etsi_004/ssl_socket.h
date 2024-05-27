#ifndef ETSI_GS_QKD_004_SSL_SOCKET_H
#define ETSI_GS_QKD_004_SSL_SOCKET_H

#include <openssl/ssl.h>
#include "utils.h"

#ifdef __cplusplus__
extern "C" {
#endif

typedef qkd_bytes_t socket_data_t;

socket_data_t socket_ssl_recv(SSL *ssl, size_t buffer_max_size);

void socket_ssl_send(SSL *ssl, socket_data_t data_bytes);

enum role_t {
    ROLE_CLIENT, ROLE_SERVER
};

SSL_CTX *socket_ssl_context_create(enum role_t role, const char *cert_pem, const char *key_pem, const char *peer_ca_pem,
                                   int max_proto_version);

void socket_ssl_context_load_certs(SSL_CTX *ctx, const char *cert_file, const char *key_file);

void socket_ssl_context_load_peer_CA(SSL_CTX *ctx, const char *peer_ca_file, enum role_t role);

SSL *socket_ssl_init(SSL_CTX *ctx, int socket);

int socket_create_connection(const char *host, uint16_t port);

SSL *
socket_client_init(const char *host, uint16_t port, const char *cert_pem, const char *key_pem, const char *peer_ca_pem,
                   int max_proto_version);

int socket_create_server(const char *host, uint16_t port);

enum server_status_t {
    SERVER_FINISHED, SERVER_LISTENING
};

void socket_server_init(const char *host, uint16_t port, const char *cert_pem, const char *key_pem, const char *ca_pem,
                        int max_proto_version,
                        void (*servlet)(SSL *ssl_socket, void *server_context_data, int *server_status),
                        void *server_context_data);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_SSL_SOCKET_H
