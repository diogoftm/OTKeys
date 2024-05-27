#include "memory_free_utils.h"

#include <stdlib.h>

void free_qkd_bytes(qkd_bytes_t* bytes) {
    if (bytes != NULL) {
        free(bytes->data);
        bytes->data = NULL;
    }
    return;
}

void free_qkd_qos(qkd_qos_t* qos) {
    if (qos != NULL) {
        free(qos->metadata_mimetype);
        qos->metadata_mimetype = NULL;
    }
    return;
}

void free_qkd_open_connect_request(qkd_open_connect_request_t* oc_request) {
    if (oc_request != NULL) {
        free(oc_request->source);
        oc_request->source = NULL;
        free(oc_request->destination);
        oc_request->destination = NULL;
        free_qkd_qos(&oc_request->qos);
    }
    return;
}

void free_qkd_open_connect_response(qkd_open_connect_response_t* oc_response) {
    free_qkd_qos(&oc_response->qos);
    return;
}

void free_qkd_get_key_request(qkd_get_key_request_t* gk_request) {
    free_qkd_bytes(&gk_request->metadata);
    return;
}

void free_qkd_get_key_response(qkd_get_key_response_t* gk_response) {
    free_qkd_bytes(&gk_response->key_buffer);
    free_qkd_bytes(&gk_response->metadata);
    return;
}