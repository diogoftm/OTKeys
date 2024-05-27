#include <malloc.h>

#include "../../include/etsi_004/copy.h"
#include "../../include/etsi_004/sizes.h"

void qkd_copy_str(char **dst, char **src) {
    size_t str_size = qkd_sizeof_str(*src);
    if (*dst != NULL) free(*dst);
    *dst = malloc(str_size);
    memcpy(*dst, *src, str_size);
}

void qkd_copy_uri(qkd_uri_t *dst, qkd_uri_t *src) {
    qkd_copy_str(dst, src);
}

void qkd_copy_mimetype(qkd_mimetype_t *dst, qkd_mimetype_t *src) {
    qkd_copy_str(dst, src);
}

void qkd_copy_qos(qkd_qos_t *dst, qkd_qos_t *src) {
    dst->key_chunk_size = src->key_chunk_size;
    dst->max_bps = src->max_bps;
    dst->min_bps = src->min_bps;
    dst->jitter = src->jitter;
    dst->priority = src->priority;
    dst->timeout = src->timeout;
    dst->ttl = src->ttl;
    qkd_copy_mimetype(&dst->metadata_mimetype, &src->metadata_mimetype);
}

