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

typedef struct sge_window {
        sge_platform_handle     handle;
        int                     width;
        int                     height;
} sge_window;

sge_window *sge_window_create(const int width,const int height,const char *window_name);
void sge_window_destroy(sge_window *window);
SGE_BOOL sge_window_show(sge_window *window);
SGE_BOOL sge_window_hide(sge_window *window);
void update_frame(const int target_fps, DWORD start_time_ms, sge_window *window);
void hide_mouse();
void show_mouse();
bool window_should_close();
mouse_pos screen_to_window(sge_render *render, mouse_pos screen_pos);
#endif //SGE_WINDOW_H
