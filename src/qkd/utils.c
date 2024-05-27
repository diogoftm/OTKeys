
#include <stdio.h>
#include "../../include/etsi_004/utils.h"

void qkd_fprintf_bytes(FILE *str, qkd_bytes_t data, int group_size) {
    fprintf(str, "Data (%u bytes): ", data.size);
    uint8_t *data_b = data.data;
    int i;
    for (i = 0; i < data.size; i++) {
        if (group_size > 0 && i % group_size == 0) fprintf(str, "\n");
        fprintf(str, "%02X ", data_b[i]);
    }
    fprintf(str, "\n");
}

void qkd_fprintf_qos(FILE *str, qkd_qos_t qos) {
    fprintf(str, "[%u, %u, %u, %u, %u, %u, %u, \"%s\"]",
            qos.key_chunk_size, qos.max_bps, qos.min_bps, qos.jitter,
            qos.priority, qos.timeout, qos.ttl, qos.metadata_mimetype);
}

void qkd_fprintf_uuid(FILE *str, uuid_t uuid) {
    char uuid_s[UUID_STR_LEN];
    uuid_unparse_lower(uuid, uuid_s);
    fprintf(str, "\"%s\"", uuid_s);
}

void qkd_fprintf_key_buffer(FILE *str, qkd_key_buffer_t key_buffer) {
    if (key_buffer.size < 1) {
        fprintf(str, "[]");
        return;
    }

    uint8_t *data = key_buffer.data;
    fprintf(str, "[%u", data[0]);
    for (int i = 1; i < key_buffer.size; i++) fprintf(str, ", %u", data[i]);
    fprintf(str, "]");
}

void qkd_fprintf_oc_request(FILE *str, qkd_open_connect_request_t oc_req) {
    fprintf(str, "{\"source\": \"%s\", \"destination\": \"%s\", \"qos\": ",
            oc_req.source, oc_req.destination);
    qkd_fprintf_qos(str, oc_req.qos);
    fprintf(str, ", \"key_stream_id\": ");
    qkd_fprintf_uuid(str, oc_req.key_stream_id);
    fprintf(str, "}");
}

void qkd_fprintf_oc_response(FILE *str, qkd_open_connect_response_t oc_res) {
    fprintf(str, "{\"status\": %d, \"qos\": ", oc_res.status);
    qkd_fprintf_qos(str, oc_res.qos);
    fprintf(str, ", \"key_stream_id\": ");
    qkd_fprintf_uuid(str, oc_res.key_stream_id);
    fprintf(str, "}");
}

void qkd_fprintf_gk_request(FILE *str, qkd_get_key_request_t gk_req) {
    fprintf(str, "{\"key_stream_id\": ");
    qkd_fprintf_uuid(str, gk_req.key_stream_id);
    fprintf(str, ",  \"index\": %u, \"metadata\": %.*s}",
            gk_req.index, (int) gk_req.metadata.size, (char *) gk_req.metadata.data);
}

void qkd_fprintf_gk_response(FILE *str, qkd_get_key_response_t gk_res) {
    fprintf(str, "{\"status\": %d, \"index\": %u, \"key_buffer\": ",
            gk_res.status, gk_res.index);
    qkd_fprintf_key_buffer(str, gk_res.key_buffer);
    fprintf(str, ", \"metadata\": %.*s}",
            (int) gk_res.metadata.size, (char *) gk_res.metadata.data);
}

void qkd_fprintf_cl_request(FILE *str, qkd_close_request_t cl_req) {
    fprintf(str, "{\"key_stream_id\": ");
    qkd_fprintf_uuid(str, cl_req.key_stream_id);
    fprintf(str, "}");
}

void qkd_fprintf_cl_response(FILE *str, qkd_close_response_t cl_res) {
    fprintf(str, "{\"status\": %d}", cl_res.status);
}

void qkd_fprintf_header(FILE *str, qkd_header_t header) {
    fprintf(str, "{\"version\": [%u, %u, %u], \"service\": %u, \"payload_size\": %u}",
            header.version.major, header.version.minor, header.version.micro, header.service, header.payload_size);
}