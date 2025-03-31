//
// Created by Geisthardt on 26.02.2025.
//

#include "core/logging.h"

#include <bemapiset.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include "utils/sge_time.h"
#include "utils/sge_file.h"
#include "utils/sge_string.h"
#include "utils/sge_utils.h"
#include "core/memory_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/sge_internal_logging.h"

const int LOG_BUFFER_SIZE = 4096;
const int TEMP_BUFFER_SIZE = 2048;

bool is_debug_2 = true;

FILE *log_file;
char *log_file_filepath;
char *log_buffer;
char *temp_buffer;
static int temp_buffer_used;
static bool cleared_temp_buffer = false;
bool started_logging = false;
bool using_logging = false;
int log_buffer_index = 0;

SGE_BOOL always_write = SGE_FALSE;
SGE_BOOL include_internal_logs = SGE_TRUE;
SGE_BOOL is_release = SGE_FALSE;

char *log_levels[] = {"FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE", NULL};
int level_padding;

int write_buffer_to_log_file();

SGE_RESULT start_logger(sge_log_settings settings) {
        using_logging = true;
        level_padding = get_longest_element_in_array(log_levels);
        log_internal_event(LOG_LEVEL_INFO, "Initializing Logger");
        log_buffer = allocate_memory(LOG_BUFFER_SIZE, MEMORY_TAG_LOGGER);
        started_logging = TRUE;
        log_internal_event(LOG_LEVEL_INFO, "Log started");
        char filename[100];
        char filepath[200];
        filepath[0] = '\0';
        const char *formatted_now_time = current_time_formatted();
        strcpy(filename, formatted_now_time);
        filename[strlen(filename)+ 1] = '\0';
        make_filename_save(filename);
        strcat(filename + strlen(filename), ".log");
        filename[strlen(filename)+ 1] = '\0';

        const char *current_working_directory = get_current_working_directory();
        //printf("%s\n", current_working_directory);
        strcat(filepath, current_working_directory);
        //printf("%s\n", filepath);
        strcat(filepath, "\\");
        strcat(filepath, "logs");

        //printf("FILEPATH: %s\n", filepath);

        char temp[32];
        if (create_directory_if_not_exists(filepath) != SGE_TRUE) {
                printf("Failed creating directory for logger\n");
                return 1;
        }
        const int current_year = get_current_year();
        snprintf(temp, sizeof(temp), "\\%d", current_year);
        strcat(filepath, temp);
        if (create_directory_if_not_exists(filepath) != SGE_TRUE) {
                printf("Failed creating directory for logger\n");
                return 1;
        }
        const int current_month = get_current_month();
        snprintf(temp, sizeof(temp), "\\%02d", current_month);
        strcat(filepath, temp);
        if (create_directory_if_not_exists(filepath) != SGE_TRUE) {
                printf("Failed creating directory for logger\n");
                return 1;
        }
        const int current_day = get_current_day();
        snprintf(temp, sizeof(temp), "\\%02d", current_day);
        strcat(filepath, temp);
        if (create_directory_if_not_exists(filepath) != SGE_TRUE) {
                printf("Failed creating directory for logger\n");
                return 1;
        }
        strcat(filepath, "\\");
        strcat(filepath, filename);
        //printf("PATH: %s\n", filepath);
        log_file_filepath = allocate_memory(strlen(filepath), MEMORY_TAG_LOGGER);
        strcpy(log_file_filepath, filepath);
        log_file = fopen(filepath, "w");
        if (log_file == NULL) {
                printf("Error creating log file\n");
                return 1;
        }

        //printf("Writing to %s\n", filename);
        fprintf(log_file, "Log started at: %s\n", formatted_now_time);
        //printf("FINISHED WRITING TO %s\n", filename);

        fclose(log_file);

        if (log_buffer == NULL) {
                printf("Error setting up logging buffer\n");
                return 1;
        }

        if (settings.write_instantly) {
                always_write = SGE_TRUE;
        } else {
                always_write = SGE_FALSE;
        }
        if (settings.include_internal_logs) {
                include_internal_logs = SGE_TRUE;
        } else {
                include_internal_logs = SGE_FALSE;
        }
        if (settings.is_release) {
                is_release = SGE_TRUE;
        } else {
                is_release = SGE_FALSE;
        }

        return SGE_SUCCESS;
}





void log_event(const log_level level, const char *message, ...) {
        if (!using_logging) {
                printf("!START LOGGING BEFORE TRYING TO LOG!!\n");
                return;
        }
        if (!is_debug_2 && level != LOG_LEVEL_FATAL) {
                return;
        }

        if (is_release) {
                if (level != LOG_LEVEL_ERROR || level != LOG_LEVEL_FATAL || level != LOG_LEVEL_WARNING) {
                        return;
                }
        }

        char formatted_message[1024];
        va_list args;
        va_start(args, message);
        vsnprintf(formatted_message, sizeof(formatted_message), message, args);
        va_end(args);

        message = formatted_message;


        if (!started_logging) {
                //printf("WARNING: Logging before Logger initialization, if this happens on StartLogger, ignore\n");
                if (temp_buffer == NULL) {
                        temp_buffer = malloc(TEMP_BUFFER_SIZE);
                        if (temp_buffer == NULL) {
                                log_internal_event(LOG_LEVEL_FATAL, "FAILED TO ALLOCATE MEMORY TO TEMP BUFFER");
                                return;
                        }
                        memset(temp_buffer, 0, TEMP_BUFFER_SIZE);
                }

                const int is_fatal = (level < 1);
                const int is_error = (level < 2);

                const char *log_entry_time = current_time_formatted();
                const char *log_entry_level = log_levels[level];
                const int log_entry_size = snprintf(NULL, 0, "[TEMP] [%*s]  [%17s]  %s\n", level_padding, log_entry_level, log_entry_time, message);

                //printf("TEMP MSG: %s\n", message);

                if (log_entry_size + temp_buffer_used > TEMP_BUFFER_SIZE) {
                        printf("TEMP LOG BUFFER IS FULL, failed to init logger, DYING NOW");
                        terminate_program();
                }

                snprintf(temp_buffer + temp_buffer_used, log_entry_size + 1, "[TEMP] [%*s]  [%17s]  %s\n",
                        level_padding, log_entry_level, log_entry_time, message);
                temp_buffer_used += log_entry_size;

                if (is_fatal == 1) {
                        terminate_program();
                }

                if (is_error == 1) {
                        //todo idk what to do
                }
                return;
        }

        if (!cleared_temp_buffer) {
                //printf("COPYING TEMP BUFFER TO LOG BUFFER\n");
                //printf("TEMP BUFFER: %s\n", temp_buffer);
                strncpy(log_buffer, temp_buffer, strlen(temp_buffer) + 1);
                //printf("LOG BUFFER: %s\n", log_buffer);
                log_buffer_index = strlen(log_buffer);
                free(temp_buffer);
                temp_buffer = NULL;
                cleared_temp_buffer = true;
                log_internal_event(LOG_LEVEL_INFO, "Copied logs from temp to correct buffer");
        }





        const int is_fatal = (level < 1);
        const int is_error = (level < 2);


        //printf("MESSAGE %s", message);

        const char *log_entry_time = current_time_formatted();
        const char *log_entry_level = log_levels[level];
        const int log_entry_size = snprintf(NULL, 0, "[%*s]  [%17s]  %s\n", level_padding, log_entry_level, log_entry_time, message);

        if (log_buffer_index + log_entry_size + 1 >= LOG_BUFFER_SIZE) {
                if (write_buffer_to_log_file() != 0) {
                        stop_logger();
                        return;
                }
                log_internal_event(LOG_LEVEL_TRACE, "Buffer full, writing to file");
        }

        snprintf(log_buffer + log_buffer_index, log_entry_size + 1, "[%*s]  [%17s]  %s\n",
                level_padding, log_entry_level, log_entry_time, message);
        log_buffer_index += log_entry_size;

        if (is_fatal == 1) {
                terminate_program();
        }

        if (is_error == 1) {
                //todo idk what to do
        }

        if (always_write == SGE_TRUE) {
                write_buffer_to_log_file();
        }

        //printf("%s\n", message);
}

int write_buffer_to_log_file() {
        if (log_file_filepath == NULL || log_buffer == NULL) {
                return 1;
        }
        log_file = fopen(log_file_filepath, "a");

        if (log_file == NULL) {
                perror("Error opening log file");
                return 1;
        }

        //printf("OPENED LOG FILE\n");

        //printf("%s", log_buffer);

        fprintf(log_file, "%s", log_buffer);

        fclose(log_file);

        //printf("WROTE TO LOG FILE\n");

        log_buffer[0] = '\0';
        log_buffer_index = 0;

        return 0;
}

SGE_RESULT stop_logger() {
    // print rest of buffer into file
        if (write_buffer_to_log_file() != 0) {
                return SGE_ERROR;
        }
        log_file_filepath[0] = '\0';
        log_buffer = NULL;
        started_logging = false;

        return SGE_SUCCESS;
}