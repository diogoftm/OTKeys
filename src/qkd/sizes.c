#include "../../include/etsi_004/sizes.h"


size_t qkd_sizeof_bytes(qkd_bytes_t data) {
    return (int) sizeof(data.size) + data.size;
}

size_t qkd_sizeof_str(const char *str) {
    return strlen(str) + 1;
}

size_t qkd_sizeof_uri(qkd_uri_t uri) {
    return qkd_sizeof_str(uri);
}

size_t qkd_sizeof_mimetype(qkd_mimetype_t mimetype) {
    size_t len = qkd_sizeof_str(mimetype);
    if (len > MIMETYPE_MAX) {
        fprintf(stdout, "[Error] Mimetype exceeds the 256 maximum payload_size");
        abort();
    };
    return len;
}

size_t qkd_sizeof_qos(qkd_qos_t qos) {
    return sizeof(qos.key_chunk_size) +
           sizeof(qos.max_bps) +
           sizeof(qos.min_bps) +
           sizeof(qos.jitter) +
           sizeof(qos.priority) +
           sizeof(qos.timeout) +
           sizeof(qos.ttl) +
           qkd_sizeof_mimetype(qos.metadata_mimetype);
}

size_t qkd_sizeof_metadata(qkd_metadata_t metadata) {
    return qkd_sizeof_bytes(metadata);
}

size_t qkd_sizeof_key_buffer(qkd_key_buffer_t key_buffer) {
    return qkd_sizeof_bytes(key_buffer);
}

size_t qkd_sizeof_open_connect_request(qkd_open_connect_request_t open_connect_request) {
    return qkd_sizeof_uri(open_connect_request.source) +
           qkd_sizeof_uri(open_connect_request.destination) +
           qkd_sizeof_qos(open_connect_request.qos) +
           sizeof(uuid_t);
}

size_t qkd_sizeof_open_connect_response(qkd_open_connect_response_t open_connect_response) {
    return sizeof(open_connect_response.status) +
           qkd_sizeof_qos(open_connect_response.qos) +
           sizeof(open_connect_response.key_stream_id);
}

size_t qkd_sizeof_get_key_request(qkd_get_key_request_t get_key_request) {
    return sizeof(get_key_request.key_stream_id) +
           sizeof(get_key_request.index) +
           qkd_sizeof_metadata(get_key_request.metadata);
}

size_t qkd_sizeof_get_key_response(qkd_get_key_response_t get_key_response) {
    return sizeof(get_key_response.status) +
           sizeof(get_key_response.index) +
           qkd_sizeof_key_buffer(get_key_response.key_buffer) +
           qkd_sizeof_metadata(get_key_response.metadata);
}

size_t qkd_sizeof_close_request(qkd_close_request_t close_request) {
    return sizeof(close_request.key_stream_id);
}

size_t qkd_sizeof_close_response(qkd_close_response_t close_response) {
    return sizeof(close_response.status);
}
