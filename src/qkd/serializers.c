#include <netinet/in.h>
#include <unistd.h>

#include "../../include/etsi_004/serializers.h"
#include "../../include/etsi_004/sizes.h"


void qkd_uint32_to_stream(uint32_t number, stream_t stream) {
    number = htonl(number);

    size_t offset = sizeof(uint32_t);
    memcpy(*stream, &number, offset);
    *stream += offset;
}

uint32_t qkd_uint32_from_stream(stream_t stream) {
    uint32_t number;

    size_t offset = sizeof(uint32_t);
    memcpy(&number, *stream, offset);
    *stream += offset;

    return ntohl(number);
}

void qkd_uuid_to_stream(uuid_t id, stream_t stream) {
    size_t offset = sizeof(uuid_t);
    memcpy(*stream, id, offset);
    *stream += offset;
}

uuid_tp qkd_uuid_from_stream(stream_t stream) {
    size_t offset = sizeof(uuid_t);
    uuid_tp id = malloc(offset);
    uuid_copy(id, *stream);
    *stream += offset;

    return id;
}

void qkd_bytes_data_to_stream(qkd_bytes_t data, stream_t stream) {
    size_t offset;

    qkd_uint32_to_stream(data.size, stream);

    offset = data.size;
    memcpy(*stream, data.data, offset);
    *stream += offset;
}

qkd_bytes_t qkd_bytes_data_from_stream(stream_t stream) {
    qkd_bytes_t data;
    size_t offset;

    data.size = qkd_uint32_from_stream(stream);

    offset = data.size;
    data.data = malloc(offset);
    memcpy(data.data, *stream, offset);
    *stream += offset;

    return data;
}

void qkd_str_to_stream(char *str, stream_t stream) {
    size_t offset;

    offset = qkd_sizeof_str(str);
    memcpy(*stream, str, offset);

    *stream += offset;
}

char *qkd_str_from_stream(stream_t stream) {
    char *str_ = *stream;
    size_t offset = qkd_sizeof_str(str_);

    char *str = malloc(offset);
    memcpy(str, *stream, offset);
    *stream += offset;

    return str;
}

void qkd_uri_to_stream(qkd_uri_t uri, stream_t stream) {
    return qkd_str_to_stream(uri, stream);
}

qkd_uri_t qkd_uri_from_stream(stream_t stream) {
    return (qkd_uri_t) qkd_str_from_stream(stream);
}

void qkd_mimetype_to_stream(char *mime, stream_t stream) {
    return qkd_str_to_stream(mime, stream);
}

qkd_mimetype_t qkd_mimetype_from_stream(stream_t stream) {
    return (qkd_mimetype_t) qkd_str_from_stream(stream);
}

void qkd_qos_to_stream(qkd_qos_t qos, stream_t stream) {
    qkd_uint32_to_stream(qos.key_chunk_size, stream);
    qkd_uint32_to_stream(qos.max_bps, stream);
    qkd_uint32_to_stream(qos.min_bps, stream);
    qkd_uint32_to_stream(qos.jitter, stream);
    qkd_uint32_to_stream(qos.priority, stream);
    qkd_uint32_to_stream(qos.timeout, stream);
    qkd_uint32_to_stream(qos.ttl, stream);
    qkd_mimetype_to_stream(qos.metadata_mimetype, stream);
}

qkd_qos_t qkd_qos_from_stream(stream_t stream) {
    qkd_qos_t qos;
    qos.key_chunk_size = qkd_uint32_from_stream(stream);
    qos.max_bps = qkd_uint32_from_stream(stream);
    qos.min_bps = qkd_uint32_from_stream(stream);
    qos.jitter = qkd_uint32_from_stream(stream);
    qos.priority = qkd_uint32_from_stream(stream);
    qos.timeout = qkd_uint32_from_stream(stream);
    qos.ttl = qkd_uint32_from_stream(stream);
    qos.metadata_mimetype = qkd_mimetype_from_stream(stream);

    return qos;
}

void qkd_key_buffer_to_stream(qkd_key_buffer_t data, stream_t stream) {
    qkd_bytes_data_to_stream(data, stream);
}

qkd_key_buffer_t qkd_key_buffer_from_stream(stream_t stream) {
    return (qkd_key_buffer_t) qkd_bytes_data_from_stream(stream);
}

void qkd_metadata_to_stream(qkd_key_buffer_t data, stream_t stream) {
    qkd_bytes_data_to_stream(data, stream);
}

qkd_key_buffer_t qkd_metadata_from_stream(stream_t stream) {
    return (qkd_key_buffer_t) qkd_bytes_data_from_stream(stream);
}

/**
 *
 * @param open_connect_request
 * @param stream
 */
void qkd_open_connect_request_to_stream(qkd_open_connect_request_t open_connect_request, stream_t stream) {
    qkd_uri_to_stream(open_connect_request.source, stream);
    qkd_uri_to_stream(open_connect_request.destination, stream);
    qkd_qos_to_stream(open_connect_request.qos, stream);
    qkd_uuid_to_stream(open_connect_request.key_stream_id, stream);
}


qkd_open_connect_request_t qkd_open_connect_request_from_stream(stream_t stream) {
    qkd_open_connect_request_t open_connect_request;
    open_connect_request.source = qkd_uri_from_stream(stream);
    open_connect_request.destination = qkd_uri_from_stream(stream);
    open_connect_request.qos = qkd_qos_from_stream(stream);

    uuid_tp uuid_ptr = qkd_uuid_from_stream(stream);
    uuid_copy(open_connect_request.key_stream_id, uuid_ptr);
    free(uuid_ptr);

    return open_connect_request;
}

void qkd_open_connect_response_to_stream(qkd_open_connect_response_t open_connect_response, stream_t stream) {
    qkd_uint32_to_stream(open_connect_response.status, stream);
    qkd_qos_to_stream(open_connect_response.qos, stream);
    qkd_uuid_to_stream(open_connect_response.key_stream_id, stream);
}

qkd_open_connect_response_t qkd_open_connect_response_from_stream(stream_t stream) {
    qkd_open_connect_response_t open_connect_response;
    open_connect_response.status = qkd_uint32_from_stream(stream);
    open_connect_response.qos = qkd_qos_from_stream(stream);

    uuid_tp uuid_ptr = qkd_uuid_from_stream(stream);
    uuid_copy(open_connect_response.key_stream_id, uuid_ptr);
    free(uuid_ptr);

    return open_connect_response;
}

void qkd_get_key_request_to_stream(qkd_get_key_request_t get_key_request, stream_t stream) {
    qkd_uuid_to_stream(get_key_request.key_stream_id, stream);
    qkd_uint32_to_stream(get_key_request.index, stream);
    qkd_metadata_to_stream(get_key_request.metadata, stream);
}

qkd_get_key_request_t qkd_get_key_request_from_stream(stream_t stream) {
    qkd_get_key_request_t get_key_request;

    uuid_tp uuid_ptr = qkd_uuid_from_stream(stream);
    uuid_copy(get_key_request.key_stream_id, uuid_ptr);
    free(uuid_ptr);

    get_key_request.index = qkd_uint32_from_stream(stream);
    get_key_request.metadata = qkd_metadata_from_stream(stream);

    return get_key_request;
}

void qkd_get_key_response_to_stream(qkd_get_key_response_t get_key_response, stream_t stream) {
    qkd_uint32_to_stream(get_key_response.status, stream);
    qkd_uint32_to_stream(get_key_response.index, stream);
    qkd_key_buffer_to_stream(get_key_response.key_buffer, stream);
    qkd_metadata_to_stream(get_key_response.metadata, stream);
}

qkd_get_key_response_t qkd_get_key_response_from_stream(stream_t stream) {
    qkd_get_key_response_t get_key_response;
    get_key_response.status = qkd_uint32_from_stream(stream);
    get_key_response.index = qkd_uint32_from_stream(stream);
    get_key_response.key_buffer = qkd_key_buffer_from_stream(stream);
    get_key_response.metadata = qkd_metadata_from_stream(stream);

    return get_key_response;
}

void qkd_close_request_to_stream(qkd_close_request_t close_request, stream_t stream) {
    qkd_uuid_to_stream(close_request.key_stream_id, stream);
}

qkd_close_request_t qkd_close_request_from_stream(stream_t stream) {
    qkd_close_request_t close_request;

    uuid_tp uuid_ptr = qkd_uuid_from_stream(stream);
    uuid_copy(close_request.key_stream_id, uuid_ptr);
    free(uuid_ptr);

    return close_request;
}

void qkd_close_response_to_stream(qkd_close_response_t close_response, stream_t stream) {
    qkd_uint32_to_stream(close_response.status, stream);
}

qkd_close_response_t qkd_close_response_from_stream(stream_t stream) {
    qkd_close_response_t close_response;
    close_response.status = qkd_uint32_from_stream(stream);

    return close_response;
}

void qkd_header_to_stream(qkd_header_t header, stream_t stream) {
    header.payload_size = htonl(header.payload_size);

    size_t offset = sizeof(qkd_header_t);
    memcpy(*stream, &header, offset);
    *stream += offset;
}

qkd_header_t qkd_header_from_stream(stream_t stream) {
    qkd_header_t header;

    size_t offset = sizeof(qkd_header_t);
    memcpy(&header, *stream, offset);
    *stream += offset;
    header.payload_size = ntohl(header.payload_size);

    return header;
}
