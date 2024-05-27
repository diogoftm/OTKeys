#ifndef MEMORY_FREE_UTILS_H
#define MEMORY_FREE_UTILS_H

#include "models.h"

#ifdef __cplusplus
    extern "C" {
#endif

void free_qkd_qos(qkd_qos_t* qos);

void free_qkd_open_connect_request(qkd_open_connect_request_t* oc_request);

void free_qkd_open_connect_response(qkd_open_connect_response_t* oc_response);

void free_qkd_get_key_request(qkd_get_key_request_t* gk_request);

void free_qkd_get_key_response(qkd_get_key_response_t* gk_response);


#ifdef __cplusplus
    }
#endif

#endif