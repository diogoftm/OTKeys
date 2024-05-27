#include <openssl/err.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <libexplain/connect.h>
#include <stdbool.h>

#include "../../include/etsi_004/ssl_socket.h"


void socket_ssl_check_errors(SSL *ssl, int len) {
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
            case SSL_ERROR_WANT_WRITE:
            case SSL_ERROR_WANT_READ:
                return;
            case SSL_ERROR_ZERO_RETURN:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
            default:
                ERR_print_errors_fp(stderr);
        }
    }
}

socket_data_t socket_ssl_recv(SSL *ssl, size_t buffer_max_size) {
    if (buffer_max_size == 0) {
        buffer_max_size = SSL3_RT_MAX_PLAIN_LENGTH;
    }
    uint8_t buf[buffer_max_size];
    size_t buf_len = 0;
    bool need_more_reads = false;

    while (buf_len < buffer_max_size) {
#ifdef QKD_LOG_DEBUG
        fprintf(stdout, "[DEBUG] Receiving");
#endif
        if (need_more_reads)
            fprintf(stdout,
                    "Warning: SSL socket has only partially read the packet, trying to read the remaining part.");
        size_t len = SSL_read(ssl, buf, (int) (buffer_max_size - buf_len));
        if (len == 0) {
            fprintf(stdout, "SSL socket error: socket closed by the peer. In the client side using TLS 1.3, this error"
                            " could represent that the server needs the client send valid certificate");
            exit(EXIT_FAILURE);
        }
        socket_ssl_check_errors(ssl, (int) len);
        buf_len += len;
        need_more_reads = true;
        if (buffer_max_size == SSL3_RT_MAX_PLAIN_LENGTH) break;
#ifdef QKD_LOG_DEBUG
        fprintf(stdout, " (%zu/%zu)", buf_len, buffer_max_size);
#endif
    }

    socket_data_t msg = {buf_len};
    msg.data = malloc(buf_len);
    memcpy(msg.data, buf, buf_len);

#ifdef QKD_LOG_DEBUG
    fprintf(stdout, ": ");
    qkd_fprintf_key_buffer(stdout, msg);
    fprintf(stdout, "\n");
#endif
    return msg;
}

void socket_ssl_send(SSL *ssl, socket_data_t data_bytes) {
#ifdef QKD_LOG_DEBUG
    fprintf(stdout, "[DEBUG] Sending (%d): ", data_bytes.size);
#endif
    if (data_bytes.size == 0) return;

    int len = SSL_write(ssl, data_bytes.data, (int) data_bytes.size);
    if (data_bytes.size != len) {
        fprintf(stdout, "SSL socket error: data_bytes sent payload_size (%u) differs from data_bytes sent (%d).\n",
                data_bytes.size,
                len);
        socket_ssl_check_errors(ssl, len);
    }
#ifdef QKD_LOG_DEBUG
    qkd_fprintf_key_buffer(stdout, data_bytes);
    fprintf(stdout, "\n");
#endif
}

void log_ssl() {
    unsigned long err;
    while ((err = ERR_get_error())) {
        char *err_msg = ERR_error_string(err, 0);
        if (!err_msg)
            return;
        ERR_print_errors_fp(stderr);
    }
}

SSL_CTX *
socket_ssl_context_create(enum role_t role, const char *cert_pem, const char *key_pem, const char *peer_ca_pem,
                          int max_proto_version) {
    if (role == ROLE_SERVER) {
        if (cert_pem == NULL || key_pem == NULL) {
            fprintf(stdout, "Error creating server context: certificate/key is mandatory for server side.\n");
            exit(EXIT_FAILURE);
        }
    }

    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = role == ROLE_CLIENT ? TLS_client_method() : TLS_server_method();

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (role == ROLE_SERVER) {
        SSL_CTX_set_ecdh_auto(ctx, 1);
    }
    SSL_CTX_set_max_proto_version(ctx, max_proto_version);

    if (ctx == NULL) {
        fprintf(stdout, "Error creating context.\n");
        log_ssl();
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_set_cipher_list(ctx, "HIGH") <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    socket_ssl_context_load_certs(ctx, cert_pem, key_pem);
    socket_ssl_context_load_peer_CA(ctx, peer_ca_pem, role);

    return ctx;
}

void socket_ssl_context_load_certs(SSL_CTX *ctx, const char *cert_file, const char *key_file) {
    if (cert_file == NULL || key_file == NULL) return;

    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
    if (!SSL_CTX_check_private_key(ctx))  // Verify private key
    {
        fprintf(stderr, "Private key does not match the certificate's public key\n");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
}

void socket_ssl_context_load_peer_CA(SSL_CTX *ctx, const char *peer_ca_file, enum role_t role) {
    if (peer_ca_file == NULL) {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
        return;
    }

    if (SSL_CTX_load_verify_locations(ctx, peer_ca_file, NULL) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
    SSL_CTX_set_verify(ctx, (role == ROLE_CLIENT) ? SSL_VERIFY_PEER : SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       NULL);
}


SSL *socket_ssl_init(SSL_CTX *ctx, int socket) {
    SSL *ssl_socket = SSL_new(ctx);
    if (!ssl_socket) {
        fprintf(stdout, "Error creating SSL.\n");
        SSL_CTX_free(ctx);
        log_ssl();
        exit(EXIT_FAILURE);
    }

    SSL_set_fd(ssl_socket, socket);
    return ssl_socket;
}

int socket_create_connection(const char *host, uint16_t port) {
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stdout, "Error creating socket.\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(host);
    sa.sin_port = htons(port);
    socklen_t socklen = sizeof(sa);
    if (connect(s, (struct sockaddr *) &sa, socklen)) {
        fprintf(stdout, "Error connecting to server \"%s:%d\": %s\n", host, port,
                explain_connect(s, (struct sockaddr *) &sa, (int) socklen));
        exit(EXIT_FAILURE);
    }
    return s;
}

SSL *
socket_client_init(const char *host, uint16_t port, const char *cert_pem, const char *key_pem,
                   const char *peer_ca_pem, int max_proto_version) {
    int s = socket_create_connection(host, port);
    SSL_CTX *ctx = socket_ssl_context_create(ROLE_CLIENT, cert_pem, key_pem, peer_ca_pem, max_proto_version);
    SSL *ssl_socket = socket_ssl_init(ctx, s);
    int err = SSL_connect(ssl_socket);
    if (err <= 0) {
        fprintf(stdout, "Error creating SSL connection.  err=%x\n", err);
        log_ssl();
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    return ssl_socket;
}

int socket_create_server(const char *host, uint16_t port) {
    int server_socket;
    struct sockaddr_in addr;
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
        perror("Setting socket option SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }
    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        perror("can't bind port");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 10) != 0) {
        perror("Can't configure listening port");
        exit(EXIT_FAILURE);
    }
    return server_socket;
}

void socket_server_init(const char *host, uint16_t port, const char *cert_pem, const char *key_pem, const char *ca_pem,
                        int max_proto_version,
                        void (*servlet)(SSL *ssl_socket, void *server_context_data, int *server_status),
                        void *server_context_data) {
    if (port < 1024 && getuid() != 0)  //Only root user have the permission to run the server_socket
    {
        fprintf(stdout, "This program need to be run as \"sudo\" user for port %d"
                        " (ports below 1024 are \"privileged ports\")", port);
        exit(EXIT_SUCCESS);
    }
    fprintf(stdout, "[INFO] Running server on \"%s:%d\"...\n", host, port);
    SSL_CTX *ctx = socket_ssl_context_create(ROLE_SERVER, cert_pem, key_pem, ca_pem, max_proto_version);
    int server_socket = socket_create_server(host, port);
    int server_status = SERVER_LISTENING;
    while (server_status == SERVER_LISTENING) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_socket = accept(server_socket, (struct sockaddr *) &addr, &len);
        SSL *ssl_socket = socket_ssl_init(ctx, client_socket);
        if (SSL_accept(ssl_socket) == -1) {
            ERR_print_errors_fp(stderr);
        } else {
            servlet(ssl_socket, server_context_data, &server_status);
        }
        SSL_shutdown(ssl_socket);
        SSL_free(ssl_socket);
        close(client_socket);
    }
    close(server_socket);
    SSL_CTX_free(ctx);
}
