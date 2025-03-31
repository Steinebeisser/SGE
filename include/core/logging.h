//
// Created by Geisthardt on 26.02.2025.
//

#ifndef LOGGING_H
#define LOGGING_H
#include <sge_types.h>

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

typedef struct sge_log_settings {
        SGE_BOOL        write_instantly;
        SGE_BOOL        include_internal_logs;
        SGE_BOOL        is_release;
} sge_log_settings;

SGE_RESULT start_logger(sge_log_settings settings);
SGE_RESULT stop_logger();
void log_event(log_level level, const char *message, ...);
#endif //LOGGING_H1
