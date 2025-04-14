//
// Created by Geisthardt on 07.04.2025.
//

#include "utils/hash/sge_crc32.h"

#include <utils/sge_math.h>

uint32_t calculate_crc32(void *data, size_t length) {
        uint32_t crc = 0xFFFFFFFF;
        uint8_t *bytes = (uint8_t *)data;

        for (size_t i = 0; i < length; i++) {
                uint8_t byte = bytes[i];
                crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ byte];
        }

        return crc ^ 0xFFFFFFFF;
}
