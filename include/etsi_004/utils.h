#ifndef ETSI_GS_QKD_004_UTILS_H
#define ETSI_GS_QKD_004_UTILS_H

#include "models.h"

#ifdef __cplusplus__
extern "C" {
#endif

void qkd_fprintf_bytes(FILE *str, qkd_bytes_t data, int group_size);

void qkd_fprintf_qos(FILE *str, qkd_qos_t qos);

void qkd_fprintf_uuid(FILE *str, uuid_t uuid);

void qkd_fprintf_key_buffer(FILE *str, qkd_key_buffer_t key_buffer);

void qkd_fprintf_oc_request(FILE *str, qkd_open_connect_request_t oc_req);

void qkd_fprintf_oc_response(FILE *str, qkd_open_connect_response_t oc_res);

void qkd_fprintf_gk_request(FILE *str, qkd_get_key_request_t gk_req);

void qkd_fprintf_gk_response(FILE *str, qkd_get_key_response_t gk_res);

void qkd_fprintf_cl_request(FILE *str, qkd_close_request_t cl_req);

void qkd_fprintf_cl_response(FILE *str, qkd_close_response_t cl_res);

void qkd_fprintf_header(FILE *str, qkd_header_t header);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_UTILS_H
