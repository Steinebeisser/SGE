//
// Created by Geisthardt on 27.02.2025.
//

#ifndef SGE_WINDOW_H
#define SGE_WINDOW_H
#include <stdbool.h>
#include <windows.h>
#include "sge_types.h"


typedef struct mouse_pos mouse_pos;
typedef struct sge_render sge_render;

typedef struct sge_screen_data{
        int     width;
        int     height;
        char    name[64];
        bool    is_primary;
        int     left;
        int     right;
        int     bottom;
        int     top;
} sge_screen_data;

typedef struct sge_screens_data{
        sge_screen_data     *screens;
        int             num_screens;
} sge_screens_data;

#ifdef WIN32
        typedef struct sge_platform_handle {
                HWND hwnd;
                HINSTANCE hinstance;
        } sge_platform_handle;
#elif UNIX
        typedef struct sge_platform_handle {
                //whatever unix needs
        } sge_platform_handle
#else

#endif


typedef enum SGE_WINDOW_MODE {
        SGE_WINDOW_MODE_WINDOWED,
        SGE_WINDOW_MODE_BORDERLESS,
        SGE_WINDOW_MODE_TRUE_FULLSCREEN,
        SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN,
        SGE_WINDOW_MODE_WINDOWED_FULLSCREEN,
} SGE_WINDOW_MODE;


typedef struct sge_window {
        sge_platform_handle     handle;
        int                     width;
        int                     height;
        int                     x;
        int                     y;
        SGE_WINDOW_MODE         current_window_mode;
        sge_screen_data         *usage_screen;
        SGE_BOOL                is_resizable;
} sge_window;

typedef struct sge_window_create_settings {
        SGE_WINDOW_MODE         window_mode;
        SGE_BOOL                is_resizable;
        int                     x;
        int                     width;
        int                     height;
        int                     y;
        sge_screen_data         *screen; //for fullscreen to know which window to choose, defaults to primary
} sge_window_create_settings;

sge_window *sge_window_create(const char *window_name, sge_window_create_settings *settings);
SGE_RESULT sge_window_destroy(sge_window *window);
SGE_BOOL sge_window_show(sge_window *window);
SGE_BOOL sge_window_hide(sge_window *window);
void update_frame(int target_fps, DWORD start_time_ms, sge_window *window);
SGE_RESULT sge_window_mode_update(sge_window *window, sge_window_create_settings *settings);
void hide_mouse();
void show_mouse();
SGE_BOOL window_should_close();
mouse_pos screen_to_window(sge_render *render, mouse_pos screen_pos);
sge_screens_data *get_screens_data();
#endif //SGE_WINDOW_H
