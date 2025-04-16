//
// Created by Geisthardt on 15.04.2025.
//

#ifndef SGE_PNG_H
#define SGE_PNG_H

#define PNG_FILE_SIGNATURE "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"
#define PNG_FILE_SIGNATURE_SIZE 8
#define PNG_FILE_CHUNK_TYPE_IHDR 0x49484452
#define PNG_FILE_CHUNK_TYPE_IDAT 0x49444154
#define PNG_FILE_CHUNK_TYPE_PLTE 0x504C5445
#define PNG_FILE_CHUNK_TYPE_IEND 0x49454E44
#define PNG_FILE_CHUNK_TYPE_SIZE 4
#define PNG_CHUNK_MAX_SAFE_SIZE (100 * 1024 * 1024) // 100 MB

#include "sge_types.h"

typedef struct sge_png_metadata {
        uint32_t        width;
        uint32_t        height;
        uint8_t         bit_depth;
        uint8_t         color_type;
        uint8_t         compression_method;
        uint8_t         filter_method;
        uint8_t         interlace_method;

        void            *idat_data;
        size_t          idat_length;

        uint8_t         *palette;
        size_t          palette_size;
} sge_png_metadata;

SGE_RESULT sge_png_parse_data(char *filename, sge_png_metadata *metadata);

#endif //SGE_PNG_H
