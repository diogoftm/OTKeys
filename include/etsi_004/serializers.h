#ifndef ETSI_GS_QKD_004_SERIALIZERS_H
#define ETSI_GS_QKD_004_SERIALIZERS_H

#include "models.h"
#include "ssl_socket.h"

#ifdef __cplusplus__
extern "C" {
#endif

typedef unsigned char *uuid_tp;
typedef void **stream_t;

void qkd_uint32_to_stream(uint32_t number, stream_t stream);

uint32_t qkd_uint32_from_stream(stream_t stream);

void qkd_uuid_to_stream(uuid_t id, stream_t stream);

uuid_tp qkd_uuid_from_stream(stream_t stream);

void qkd_bytes_data_to_stream(qkd_bytes_t data, stream_t stream);

qkd_bytes_t qkd_bytes_data_from_stream(stream_t stream);

void qkd_str_to_stream(char *str, stream_t stream);

char *qkd_str_from_stream(stream_t stream);

void qkd_uri_to_stream(qkd_uri_t uri, stream_t stream);

qkd_uri_t qkd_uri_from_stream(stream_t stream);

void qkd_mimetype_to_stream(char *mime, stream_t stream);

qkd_mimetype_t qkd_mimetype_from_stream(stream_t stream);

void qkd_qos_to_stream(qkd_qos_t qos, stream_t stream);

qkd_qos_t qkd_qos_from_stream(stream_t stream);

void qkd_key_buffer_to_stream(qkd_key_buffer_t data, stream_t stream);

qkd_key_buffer_t qkd_key_buffer_from_stream(stream_t stream);

void qkd_metadata_to_stream(qkd_key_buffer_t data, stream_t stream);

qkd_key_buffer_t qkd_metadata_from_stream(stream_t stream);

void qkd_open_connect_request_to_stream(qkd_open_connect_request_t open_connect_request, stream_t stream);

qkd_open_connect_request_t qkd_open_connect_request_from_stream(stream_t stream);

void qkd_open_connect_response_to_stream(qkd_open_connect_response_t open_connect_response, stream_t stream);

qkd_open_connect_response_t qkd_open_connect_response_from_stream(stream_t stream);

void qkd_get_key_request_to_stream(qkd_get_key_request_t get_key_request, stream_t stream);

qkd_get_key_request_t qkd_get_key_request_from_stream(stream_t stream);

void qkd_get_key_response_to_stream(qkd_get_key_response_t get_key_response, stream_t stream);

qkd_get_key_response_t qkd_get_key_response_from_stream(stream_t stream);

void qkd_close_request_to_stream(qkd_close_request_t close_request, stream_t stream);

qkd_close_request_t qkd_close_request_from_stream(stream_t stream);

void qkd_close_response_to_stream(qkd_close_response_t close_response, stream_t stream);

qkd_close_response_t qkd_close_response_from_stream(stream_t stream);

void qkd_header_to_stream(qkd_header_t header, stream_t stream);

qkd_header_t qkd_header_from_stream(stream_t stream);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_SERIALIZERS_H
