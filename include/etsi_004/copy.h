#ifndef ETSI_GS_QKD_004_COPY_H
#define ETSI_GS_QKD_004_COPY_H

#include "models.h"

#ifdef __cplusplus__
extern "C" {
#endif

void qkd_copy_str(char **dst, char **src);

void qkd_copy_uri(qkd_uri_t *dst, qkd_uri_t *src);

void qkd_copy_mimetype(qkd_mimetype_t *dst, qkd_mimetype_t *src);

void qkd_copy_qos(qkd_qos_t *dst, qkd_qos_t *src);

#ifdef __cplusplus__
}
#endif

#endif //ETSI_GS_QKD_004_COPY_H
