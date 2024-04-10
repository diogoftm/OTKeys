#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

    size_t b64_decoded_size(const char *);
    int b64_isvalidchar(char);
    int b64_decode(const char *, unsigned char *, size_t);

#ifdef __cplusplus
}
#endif

#endif