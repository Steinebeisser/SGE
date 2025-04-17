//
// Created by Geisthardt on 15.04.2025.
//

#include "utils/cpu/sge_endianess.h"

#include <assert.h>
#include <core/sge_internal_logging.h>
#include <utils/sge_utils.h>

sge_endian SGE_SYSTEM_ENDIAN;

void sge_detect_endian() {
        static uint32_t endianness = 0xdeadbeef;

        //printf("%x\n", (uint8_t)endianness);
        SGE_SYSTEM_ENDIAN = (uint8_t)endianness == 0xef ? SGE_ENDIAN_LITTLE :
                                        (uint8_t)endianness == 0xde ? SGE_ENDIAN_BIG :
                                        SGE_ENDIAN_INVALID;

        if (SGE_SYSTEM_ENDIAN == SGE_ENDIAN_INVALID) {
                log_internal_event(LOG_LEVEL_FATAL, "Wrong endianess switch to processor that supports BIG or LITTLE endian");
                terminate_program();
        }
}

void be_to_ne(void *buffer, size_t element_size, size_t count) {
        if (!SGE_SYSTEM_ENDIAN) {
                sge_detect_endian();
        }

        if (SGE_SYSTEM_ENDIAN == SGE_ENDIAN_BIG) {
                return;
        }

        uint8_t *data = (uint8_t*)(buffer);
        for (size_t i = 0; i < count; ++i) {
                uint8_t swap_element_front = 0;
                uint8_t swap_element_back = 0;
                uint8_t *element = data + i * element_size;
                for (size_t j = 0; j < element_size; ++j) {
                        if (j >= element_size / 2) {
                                break;
                        }
                        swap_element_front = element[j];
                        swap_element_back = element[element_size - 1 - j];
                        element[j] = swap_element_back;
                        element[element_size - 1 - j] = swap_element_front;
                }
        }
}

size_t fread_be(void *dst_buf, size_t element_size, size_t count, FILE *fd) {

        size_t read = fread(dst_buf, element_size, count, fd);

        be_to_ne(dst_buf, element_size, read);

        return read;
}
