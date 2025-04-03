//
// Created by Geisthardt on 28.03.2025.
//

#ifndef SGE_VERSION_H
#define SGE_VERSION_H

#include "sge_types.h"

SGE_VERSION get_sge_version();
uint32_t sge_make_app_version(uint32_t major, uint32_t minor, uint32_t patch);
uint32_t sge_extract_major(uint32_t version);
uint32_t sge_extract_minor(uint32_t version);
uint32_t sge_extract_patch(uint32_t version);
#endif //SGE_VERSION_H