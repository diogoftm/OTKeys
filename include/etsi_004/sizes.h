#ifndef ETSI_GS_QKD_004_SIZES_H
#define ETSI_GS_QKD_004_SIZES_H

#include <stddef.h>
#include <string.h>
#include "ssl_socket.h"
#include "models.h"

#ifdef __cplusplus__
extern "C" {
#endif


size_t qkd_sizeof_bytes(qkd_bytes_t data);

size_t qkd_sizeof_str(const char *str);

size_t qkd_sizeof_uri(qkd_uri_t uri);

size_t qkd_sizeof_mimetype(qkd_mimetype_t mimetype);

size_t qkd_sizeof_qos(qkd_qos_t qos);

size_t qkd_sizeof_metadata(qkd_metadata_t metadata);

size_t qkd_sizeof_key_buffer(qkd_key_buffer_t key_buffer);

size_t qkd_sizeof_open_connect_request(qkd_open_connect_request_t open_connect_request);

size_t qkd_sizeof_open_connect_response(qkd_open_connect_response_t open_connect_response);

size_t qkd_sizeof_get_key_request(qkd_get_key_request_t get_key_request);

size_t qkd_sizeof_get_key_response(qkd_get_key_response_t get_key_response);

size_t qkd_sizeof_close_request(qkd_close_request_t close_request);

size_t qkd_sizeof_close_response(qkd_close_response_t close_response);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_SIZES_H
