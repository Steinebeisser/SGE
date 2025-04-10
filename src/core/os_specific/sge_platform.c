//
// Created by Geisthardt on 06.03.2025.
//

#include "core/platform/sge_platform.h"

#include <stdint.h>

#include "core/platform/sge_window.h"
#include "core/sge_internal_logging.h"
#include "core/memory_control.h"

void os_sleep(uint32_t sleep_time_ms) {
#ifdef WIN32
        Sleep(sleep_time_ms);
#elif UNIX
        #include <unistd.h>
        log_internal_event(LOG_LEVEL_FATAL, "no unix support for sleep (os_utils.c");
#else
        log_internal_event(LOG_LEVEL_FATAL, "unsupported sleep (os_utils.c");
#endif

}