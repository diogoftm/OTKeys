#include <string.h>
#include <stdio.h>

#include "../../include/etsi_004/interfaces.h"
#include "../../include/etsi_004/copy.h"
#include "../../include/etsi_004/utils.h"


const version_t QKD_004_VERSION = {2, 1, 1};

qkd_open_connect_response_t qkd_open_connect_struct(qkd_open_connect_request_t open_connect_request,
                                                    void *app_context_data) {
    qkd_open_connect_response_t open_connect_response = {};

    // In-out params
    qkd_copy_qos(&open_connect_response.qos, &open_connect_request.qos);
    memcpy(&open_connect_response.key_stream_id, &open_connect_request.key_stream_id, sizeof(uuid_t));

    qkd_open_connect(open_connect_request.source, open_connect_request.destination, &open_connect_response.qos,
                     &open_connect_response.key_stream_id, &open_connect_response.status, app_context_data);

#ifdef QKD_LOG_DEBUG
    fprintf(stdout, "[INFO] ");
    qkd_fprintf_oc_request(stdout, open_connect_request);
    fprintf(stdout, "\n[INFO] ");
    qkd_fprintf_oc_response(stdout, open_connect_response);
    fprintf(stdout, "\n");
#endif

    return open_connect_response;
}

qkd_get_key_response_t qkd_get_key_struct(qkd_get_key_request_t get_key_request, void *app_context_data) {
    qkd_get_key_response_t get_key_response = {};

    // In-out params
    get_key_response.index = get_key_request.index;
    memcpy(&get_key_response.metadata, &get_key_request.metadata, sizeof(qkd_metadata_t));

    qkd_get_key(get_key_request.key_stream_id, &get_key_response.index, &get_key_response.key_buffer,
                &get_key_response.metadata, &get_key_response.status, app_context_data);

#ifdef QKD_LOG_DEBUG
    fprintf(stdout, "[INFO] ");
    qkd_fprintf_gk_request(stdout, get_key_request);
    fprintf(stdout, "\n[INFO] ");
    qkd_fprintf_gk_response(stdout, get_key_response);
    fprintf(stdout, "\n");
#endif

    return get_key_response;
}

qkd_close_response_t qkd_close_struct(qkd_close_request_t close_request, void *app_context_data) {
    qkd_close_response_t close_response = {};

    qkd_close(close_request.key_stream_id, &close_response.status, app_context_data);

#ifdef QKD_LOG_DEBUG
    fprintf(stdout, "[INFO] ");
    qkd_fprintf_cl_request(stdout, close_request);
    fprintf(stdout, "\n[INFO] ");
    qkd_fprintf_cl_response(stdout, close_response);
    fprintf(stdout, "\n");
#endif

    return close_response;
}