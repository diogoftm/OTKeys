#include <string.h>

#include "../../include/etsi_004/interfaces.h"
#include "../../include/etsi_004/ssl_socket.h"
#include "../../include/etsi_004/models.h"
#include "../../include/etsi_004/serializers.h"
#include "../../include/etsi_004/sizes.h"
#include "../../include/etsi_004/ssl_socket_client.h"
#include "../../include/etsi_004/copy.h"


socket_data_t client_perform_request(socket_data_t request_data, void *client_context_data) {
    qkd_client_context_data_t *cfg = client_context_data;
    SSL *ssl_socket = socket_client_init(cfg->host, cfg->port, cfg->cert_pem, cfg->key_pem, cfg->peer_ca_pem,
                                         cfg->max_proto_version);
#ifdef QKD_LOG_INFO
    fprintf(stdout, "[INFO] SSL connection on \"%s:%d\" using %s with cipher suit %s:", cfg->host, cfg->port,
            SSL_get_version(ssl_socket), SSL_get_cipher (ssl_socket));
#endif
    socket_ssl_send(ssl_socket, request_data);
    socket_data_t header_data = socket_ssl_recv(ssl_socket, sizeof(qkd_header_t));

    void *header_stream = header_data.data;
    qkd_header_t header = qkd_header_from_stream(&header_stream);
#ifdef QKD_LOG_INFO
    switch (header.service) {
        case QKD_SERVICE_OPEN_CONNECT_RESPONSE:
            fprintf(stdout, " OPEN_CONNECT");
            break;
        case QKD_SERVICE_GET_KEY_RESPONSE:
            fprintf(stdout, " GET_KEY");
            break;
        case QKD_SERVICE_CLOSE_RESPONSE:
            fprintf(stdout, " CLOSE");
            break;
    }
    fprintf(stdout, "\n");
#endif
    return socket_ssl_recv(ssl_socket, header.payload_size);
}

void qkd_open_connect(qkd_uri_t source, qkd_uri_t destination, qkd_qos_t *qos, uuid_t *key_stream_id,
                      qkd_status_t *status, void *app_context_data) {
    qkd_open_connect_request_t oc_request = {source, destination, *qos, {}};
    uuid_copy(oc_request.key_stream_id, *key_stream_id);

    qkd_header_t request_header = {QKD_004_VERSION, QKD_SERVICE_OPEN_CONNECT_REQUEST,
                                   qkd_sizeof_open_connect_request(oc_request)};

    size_t request_size = sizeof(qkd_header_t) + request_header.payload_size;
    void *request_buf = malloc(request_size);
    void *request_stream = request_buf;
    qkd_header_to_stream(request_header, (stream_t) &request_stream);
    qkd_open_connect_request_to_stream(oc_request, &request_stream);

    // Send request
    socket_data_t response_data = client_perform_request((socket_data_t) {request_size, request_buf}, app_context_data);
    if (request_buf != NULL) free(request_buf);

    void *response_stream = response_data.data;
    qkd_open_connect_response_t response = qkd_open_connect_response_from_stream(&response_stream);

    // Fill return values
    *status = response.status;
    uuid_copy(*key_stream_id, response.key_stream_id);
    qkd_copy_qos(qos, &response.qos);

    if (response_data.data != NULL) free(response_data.data);
}

void qkd_get_key(uuid_t key_stream_id, uint32_t *index, qkd_key_buffer_t *key_buffer, qkd_metadata_t *metadata,
                 qkd_status_t *status, void *app_context_data) {
    qkd_get_key_request_t gk_request = {{}, *index, *metadata};
    uuid_copy(gk_request.key_stream_id, key_stream_id);

    qkd_header_t request_header = {QKD_004_VERSION, QKD_SERVICE_GET_KEY_REQUEST,
                                   qkd_sizeof_get_key_request(gk_request)};

    size_t request_size = sizeof(qkd_header_t) + request_header.payload_size;
    void *request_buf = malloc(request_size);
    void *request_stream = request_buf;
    qkd_header_to_stream(request_header, (stream_t) &request_stream);
    qkd_get_key_request_to_stream(gk_request, &request_stream);

    // Send request
    socket_data_t response_data = client_perform_request((socket_data_t) {request_size, request_buf}, app_context_data);
    if (request_buf != NULL) free(request_buf);

    void *response_stream = response_data.data;
    
    qkd_get_key_response_t response = qkd_get_key_response_from_stream(&response_stream);

    // Fill return values
    *status = response.status;
    *index = response.index;
    *key_buffer = response.key_buffer;
    *metadata = response.metadata;

    if (response_data.data != NULL) free(response_data.data);
}

void qkd_close(uuid_t key_stream_id, qkd_status_t *status, void *app_context_data) {
    qkd_close_request_t cl_request = {{}};
    uuid_copy(cl_request.key_stream_id, key_stream_id);

    qkd_header_t request_header = {QKD_004_VERSION, QKD_SERVICE_CLOSE_REQUEST,
                                   qkd_sizeof_close_request(cl_request)};

    size_t request_size = sizeof(qkd_header_t) + request_header.payload_size;
    void *request_buf = malloc(request_size);
    void *request_stream = request_buf;
    qkd_header_to_stream(request_header, (stream_t) &request_stream);
    qkd_close_request_to_stream(cl_request, &request_stream);

    // Send request
    socket_data_t response_data = client_perform_request((socket_data_t) {request_size, request_buf}, app_context_data);
    if (request_buf != NULL) free(request_buf);

    void *response_stream = response_data.data;
    qkd_close_response_t response = qkd_close_response_from_stream(&response_stream);

    // Fill return values
    *status = response.status;

    if (response_data.data != NULL) free(response_data.data);
}
