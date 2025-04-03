//
// Created by Geisthardt on 28.03.2025.
//

#ifndef SGE_TYPES_H
#define SGE_TYPES_H

#include <stdint.h>

#define SGE_VERSION_MAJOR 0
#define SGE_VERSION_MINOR 1
#define SGE_VERSION_PATCH 1

#define SGE_NULL_PTR = 0;

typedef struct SGE_VERSION {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
} SGE_VERSION;


typedef enum SGE_RESULT {
        SGE_SUCCESS = 0,
        SGE_ERROR,
        SGE_INVALID_API_CALL,
        SGE_RESIZE,
        SGE_ERROR_INVALID_API,
        SGE_ERROR_FAILED_ALLOCATION,
        SGE_UNSUPPORTED_SYSTEM
} SGE_RESULT;

typedef enum SGE_BOOL {
        SGE_FALSE = 0,
        SGE_TRUE = 1,
} SGE_BOOL;
#endif //SGE_TYPES_H
