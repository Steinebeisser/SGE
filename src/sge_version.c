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
