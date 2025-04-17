//
// Created by Geisthardt on 15.04.2025.
//

#ifndef SGE_ENDIANESS_H
#define SGE_ENDIANESS_H
#include <stdint.h>
#include <stdio.h>

typedef enum sge_endian {
        SGE_ENDIAN_UNINITIALIZED = 0,
        SGE_ENDIAN_BIG,
        SGE_ENDIAN_LITTLE,
        SGE_ENDIAN_INVALID,
} sge_endian;

extern sge_endian SGE_SYSTEM_ENDIAN;

void sge_detect_endian();

void be_to_ne(void *buffer, size_t element_size, size_t count);
size_t fread_be(void *dst_buf, size_t element_size, size_t count, FILE *fd);

#endif //SGE_ENDIANESS_H
