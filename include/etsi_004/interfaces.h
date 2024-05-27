#ifndef ETSI_GS_QKD_004_INTERFACES_H
#define ETSI_GS_QKD_004_INTERFACES_H

#include "models.h"

#ifdef __cplusplus__
extern "C" {
#endif

extern const version_t QKD_004_VERSION;

/**
 * @brief This is the OPEN_CONNECT call of the ETSI QKD 004.
 *
 * @param source        (in) the source identifier for the stream.
 * @param destination   (in) the destination identifier for the stream.
 * @param qos           (in/out) Quality of Service parameters, an empty qos marks end of list.
 * @param key_stream_id (in/out) key stream id.
 * @param status (out) status code of this call (see qkd_status_codes enum).
 * @param app_context_data   (in/out) service shared data (non-standard parameter). Used to share data between different processes.
 */
void
qkd_open_connect(qkd_uri_t source, qkd_uri_t destination, qkd_qos_t *qos, uuid_t *key_stream_id,
                 qkd_status_t *status, void *app_context_data);


/**
 * @brief This is the GET_KEY call of the ETSI QKD 004.
 *
 * @param key_stream_id (in) the key stream id, must be retrieved by a previous qkd_open.
 * @param index         (in/out) the index within the key stream id buffer.
 * @param key_buffer    (out) the key material buffer to be filled.
 * @param metadata      (in/out) the qkd_metadata for the key.
 * @param status        (out) status code of this call (see qkd_status_codes enum).
 * @param app_context_data   (in/out) service shared data (non-standard parameter). Used to share data between different processes.
 */
void qkd_get_key(uuid_t key_stream_id, uint32_t *index, qkd_key_buffer_t *key_buffer, qkd_metadata_t *metadata,
                 qkd_status_t *status, void *app_context_data);

/**
 * @brief This is the CLOSE call of the ETSI QKD 004.
 *
 * @param key_stream_id (in) the key stream to close.
 * @param status        (out) status code of this call (see qkd_status_codes enum).
 * @param app_context_data   (in/out) service shared data (non-standard parameter). Used to share data between different processes.
 */
void qkd_close(uuid_t key_stream_id, qkd_status_t *status, void *app_context_data);


qkd_open_connect_response_t qkd_open_connect_struct(qkd_open_connect_request_t open_connect_request,
                                                    void *app_context_data);

qkd_get_key_response_t qkd_get_key_struct(qkd_get_key_request_t get_key_request, void *app_context_data);

qkd_close_response_t qkd_close_struct(qkd_close_request_t close_request, void *app_context_data);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_INTERFACES_H
