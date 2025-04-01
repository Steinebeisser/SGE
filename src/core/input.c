//
// Created by Geisthardt on 03.03.2025.
//

#include "core/input.h"

#include <stdbool.h>
#include <stdio.h>

#include "core/sge_internal_logging.h"
#include "core/memory_control.h"

int *key_states = NULL;
int *last_key_states = NULL;
int *mouse_states = NULL;
int *last_mouse_states = NULL;
SGE_BOOL is_tracking_enabled = SGE_FALSE;
mouse_pos last_mouse_pos = {0, 0};
mouse_pos delta_mouse_pos = {0,0};

SGE_RESULT enable_input_tracking() {
        is_tracking_enabled = SGE_TRUE;
        key_states = allocate_memory(256 * sizeof(int), MEMORY_TAG_INPUT);
        last_key_states = allocate_memory(256 * sizeof(int), MEMORY_TAG_INPUT);
        if (key_states == NULL || last_key_states == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to init Key states");
                return SGE_ERROR_FAILED_ALLOCATION      ;
        }
        mouse_states = allocate_memory(16 * sizeof(int), MEMORY_TAG_INPUT);
        last_mouse_states = allocate_memory(16 * sizeof(int), MEMORY_TAG_INPUT);
        if (mouse_states == NULL || last_mouse_states == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to init Mouse states");
                return SGE_ERROR_FAILED_ALLOCATION;
        }

        return SGE_SUCCESS;
}


mouse_pos get_mouse_position() {
        return last_mouse_pos;
}

mouse_pos get_window_mouse_pos(sge_render *render, mouse_pos screen_pos) {
        return screen_to_window(render, screen_pos);
}

mouse_pos get_delta_mouse_position() {
        return delta_mouse_pos;
}

SGE_BOOL is_key_down(const keys key) {
        if (!is_tracking_enabled) {
                return SGE_FALSE;
        }
        //printf("CHECKING KEY: %d", key);
        return key_states[key];
}

SGE_BOOL is_key_pressed(const keys key) {
        if (!is_tracking_enabled) {
                return SGE_FALSE;
        }
        if (last_key_states[key]) {
                //prSGE_BOOLf("LAST KEY STATE: %d", last_key_states[key]);
                //prSGE_BOOLf("ALREADY PRESSED\n");
                return SGE_FALSE;
        }
        if (key_states[key]) {
                return SGE_TRUE;
        }
        return SGE_FALSE;
}

SGE_BOOL is_mouse_down(mouse_buttons button) {
        if (!is_tracking_enabled) {
                return SGE_FALSE;
        }
        //prSGE_BOOLf("CHECKING KEY: %d", key);
        return mouse_states[button];
}

SGE_BOOL was_mouse_down(mouse_buttons button) {
        if (!is_tracking_enabled) {
                return SGE_FALSE;
        }

        if (!mouse_states[button]) {
                return last_mouse_states[button];
        }

        return SGE_FALSE;
}
SGE_BOOL is_mouse_pressed(mouse_buttons button) {
        if (!is_tracking_enabled) {
                return SGE_FALSE;
        }
        if (last_mouse_states[button]) {
                //prSGE_BOOLf("LAST KEY STATE: %d", last_key_states[key]);
                //prSGE_BOOLf("ALREADY PRESSED\n");
                return SGE_FALSE;
        }
        if (mouse_states[button]) {
                return SGE_TRUE;
        }
        return SGE_FALSE;
}

void update_key_states() {
        //printf("Updating last key states\n");
        copy_memory(last_key_states, key_states, 256*sizeof(int), 0, 0);
        copy_memory(last_mouse_states, mouse_states, 16*sizeof(int), 0,0);
}