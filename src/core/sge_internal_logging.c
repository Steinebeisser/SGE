//
// Created by Geisthardt on 31.03.2025.
//

#include "core/sge_internal_logging.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern bool using_logging;

void log_internal_event(const log_level level, const char *message, ...) {
        if (!using_logging) {
                return;
        }

        const char *prefix = "[INTERNAL] ";
        char formatted_message[1024];

        strcpy(formatted_message, prefix);

        va_list args;
        va_start(args, message);
        vsnprintf(formatted_message + strlen(prefix), sizeof(formatted_message) - strlen(prefix), message, args);
        va_end(args);

        log_event(level, "%s", formatted_message);
}
