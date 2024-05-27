#ifndef ETSI_GS_QKD_004_MODELS_H
#define ETSI_GS_QKD_004_MODELS_H

#include <stdint.h>
#include <uuid/uuid.h>

#ifdef __cplusplus__
extern "C" {
#endif

typedef struct qkd_bytes {
    uint32_t size;
    void *data;
} qkd_bytes_t;

/**
 * @brief ETSI SG QKD 004 status codes.
 */
typedef enum qkd_status_code {
    qkd_status_code_successful = 0,                    /**< @brief Successful. */
    qkd_status_code_peer_not_connected = 1,            /**< @brief Successful connection, but peer not connected. */
    qkd_status_code_gk_insufficient_key_available = 2, /**< @brief GET_KEY failed because insufficient key available. */
    qkd_status_code_gk_peer_not_connected = 3,         /**< @brief GET_KEY failed because peer application is not yet connected. */
    qkd_status_code_no_qkd_connection_available = 4,   /**< @brief No QKD connection available. */
    qkd_status_code_oc_ksid_already_in_use = 5,        /**< @brief OPEN_CONNECT failed because the "KSID" is already in use. */
    qkd_status_code_timeout_error = 6,                 /**< @brief TIMEOUT_ERROR The call failed because the specified TIMEOUT. */
    qkd_status_code_oc_qos_not_satisfiable = 7,        /**< @brief OPEN_CONNECT failed because requested QoS settings could not be met, counter proposal included in returnhas occurred. */
    qkd_status_code_gk_metadata_insufficient_size = 8, /**< @brief GET_KEY failed because metadata field payload_size insufficient. Returned Metadata_size value holds minimum needed payload_size of metadata. */
} qkd_status_code_t;

typedef enum qkd_key_info_role {
    qkd_key_type_role_default = 0x00,
    qkd_key_type_role_error = 0x01,
    qkd_key_type_role_sender = 0x10,
    qkd_key_type_role_receiver = 0x11,
} qkd_key_info_role_t;

typedef enum qkd_key_info_type {
    qkd_key_type_symmetric = 0x00,
    qkd_key_type_oblivious = 0x01,
} qkd_key_info_type_t;

/**
 * @brief All status codes are 32 bits unsigned integers.
 */
typedef uint32_t qkd_status_t;

/**
 * @brief A qkd_uri is a sized string.
 */
typedef char *qkd_uri_t;

#define MIMETYPE_MAX 256

/**
 * @brief A mimetype is a string with a maximum payload_size of 255 char + '\0'.
 */
typedef char *qkd_mimetype_t;


/**
 * @brief QoS parameters.
 */
typedef struct qkd_qos {
    uint32_t key_chunk_size;
    uint32_t max_bps;
    uint32_t min_bps;
    uint32_t jitter;
    uint32_t priority;
    uint32_t timeout;
    uint32_t ttl;
    qkd_mimetype_t metadata_mimetype;
} qkd_qos_t;


/**
 * @brief The qkd_metadata value buffer.
 */
typedef qkd_bytes_t qkd_metadata_t;


/**
 * @brief   A key buffer.
 */
typedef qkd_bytes_t qkd_key_buffer_t;


typedef struct qkd_open_connect_request {
    qkd_uri_t source;
    qkd_uri_t destination;
    qkd_qos_t qos;
    uuid_t key_stream_id;
} qkd_open_connect_request_t;

typedef struct qkd_open_connect_response {
    qkd_status_t status;
    qkd_qos_t qos;
    uuid_t key_stream_id;
} qkd_open_connect_response_t;

typedef struct qkd_get_key_request {
    uuid_t key_stream_id;
    uint32_t index;
    qkd_metadata_t metadata;
} qkd_get_key_request_t;

typedef struct qkd_get_key_response {
    qkd_status_t status;
    uint32_t index;
    qkd_key_buffer_t key_buffer;
    qkd_metadata_t metadata;
} qkd_get_key_response_t;

typedef struct qkd_close_request {
    uuid_t key_stream_id;
} qkd_close_request_t;

typedef struct qkd_close_response {
    qkd_status_t status;
} qkd_close_response_t;

enum qkd_service {
    QKD_SERVICE_ERROR = 0x00,
    QKD_SERVICE_OPEN_CONNECT_REQUEST = 0b10,
    QKD_SERVICE_OPEN_CONNECT_RESPONSE = 0b11,
    QKD_SERVICE_GET_KEY_REQUEST = 0b100,
    QKD_SERVICE_GET_KEY_RESPONSE = 0b101,
    QKD_SERVICE_CLOSE_REQUEST = 0b1000,
    QKD_SERVICE_CLOSE_RESPONSE = 0b1001,
    QKD_SERVICE_CLOSE_SERVER = 0xFF,
};

typedef struct version {
    uint8_t major;
    uint8_t minor;
    uint8_t micro;
} version_t;

typedef struct qkd_header {
    version_t version;
    uint8_t service;
    uint32_t payload_size;
} qkd_header_t;

typedef uint8_t qkd_key_type_t;
typedef uint8_t qkd_key_role_t;

typedef struct qkd_key_info{
    qkd_key_type_t key_type;
    qkd_key_role_t key_role;
} qkd_key_info_t;

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_MODELS_H
