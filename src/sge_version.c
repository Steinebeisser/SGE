//
// Created by Geisthardt on 28.03.2025.
//

#include "sge_version.h"

SGE_VERSION get_sge_version() {
        const SGE_VERSION current_version = {
                .major = SGE_VERSION_MAJOR,
                .minor = SGE_VERSION_MINOR,
                .patch = SGE_VERSION_PATCH
        };

        return current_version;
}

uint32_t sge_make_app_version(uint32_t major, uint32_t minor, uint32_t patch) {
        return major << 22U | minor << 12U | patch;
}

uint32_t sge_extract_major(uint32_t version) {
        return version >> 22 & 0x3FF;
}

uint32_t sge_extract_minor(uint32_t version) {
        return version >> 12 & 0x3FF;
}

uint32_t sge_extract_patch(uint32_t version) {
        return version & 0xFFF;
}
