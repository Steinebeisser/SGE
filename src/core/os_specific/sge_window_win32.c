//
// Created by Geisthardt on 27.02.2025.
//

#include <stdio.h>
#include <windows.h>


#include "core/platform/sge_window.h"
#include "core/sge_internal_logging.h"
#include "utils/sge_time.h"
#include "core/input.h"
#include <windowsx.h>

#include "core/platform/sge_platform.h"
#include "core/memory_control.h"
#include "renderer/sge_render.h"
#include "../../renderer/vulkan_renderer/vulkan_renderer.h"
#ifdef WIN32

size_t g_delta_time = 0;
int is_window_open = 0;
int g_fps = 0;
int width;
int height;
bool is_resize = false;

bool is_hidden = false;
bool has_changed = false;
bool ignore_next_mousemove = true;
SGE_BOOL tracki_mousi = SGE_FALSE;
mouse_pos last_visible_pos = {0,0};

extern SGE_BOOL is_tracking_enabled;


mouse_pos get_window_center(HWND hwnd) {
        RECT client_rect;
        POINT center_point;

        // Get the client area (without borders/title bar)
        if (GetClientRect(hwnd, &client_rect)) {
                center_point.x = client_rect.left + (client_rect.right - client_rect.left) / 2;
                center_point.y = client_rect.top + (client_rect.bottom - client_rect.top) / 2;

                // Convert client-area coordinates to screen coordinates
                ClientToScreen(hwnd, &center_point);

                mouse_pos center_mouse_pos = { center_point.x, center_point.y };
                return center_mouse_pos;
        }

        return (mouse_pos){0, 0};  // Return (0,0) on failure
}

mouse_pos screen_to_window(sge_render *render, mouse_pos screen_pos) {
        HWND hwnd = render->window->handle.hwnd;
        POINT screen_point = {
                .x = screen_pos.x,
                .y = screen_pos.y
        };
        ScreenToClient(hwnd, &screen_point);

        mouse_pos window_pos = {
                .x = screen_point.x,
                .y = screen_point.y
        };

        if (window_pos.x >= 0 && window_pos.x < render->window->width &&
                window_pos.y >= 0 && window_pos.y < render->window->height) {
                return window_pos;
        } else {
                window_pos.x = -1000;
                window_pos.y = -1000;
                return window_pos;
        }
}

BOOL CALLBACK MonitorEnumProc(
        HMONITOR hMonitor,
        HDC hdcMonitor,
        LPRECT lprcMonitor,
        LPARAM dwData
) {
        sge_screens_data *data = (sge_screens_data*)dwData;

        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);

        if (GetMonitorInfo(hMonitor, (LPMONITORINFO)&monitorInfo)) {
                int width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
                int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

                sge_screen_data *new_screens = realloc(data->screens, (data->num_screens + 1) * sizeof(sge_screen_data));
                if (new_screens == NULL) {
                    return FALSE;
                }
                data->screens = new_screens;

                sge_screen_data *new_screen = &data->screens[data->num_screens];
                new_screen->is_primary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
                new_screen->width = width;
                new_screen->height = height;
                new_screen->bottom = monitorInfo.rcMonitor.bottom;
                new_screen->top = monitorInfo.rcMonitor.top;
                new_screen->left = monitorInfo.rcMonitor.left;
                new_screen->right = monitorInfo.rcMonitor.right;

                DISPLAY_DEVICE dd;
                dd.cb = sizeof(DISPLAY_DEVICE);
                if (EnumDisplayDevices(monitorInfo.szDevice, 0, &dd, 0)) {
                        strncpy(new_screen->name, dd.DeviceString, 63);
                        new_screen->name[63] = '\0';
                } else {
                        strncpy(new_screen->name, monitorInfo.szDevice, 63);
                        new_screen->name[63] = '\0';
                }

                data->num_screens++;
        }

        return TRUE;
}

sge_screens_data *get_screens_data() {
        sge_screens_data *screens_data = allocate_memory(sizeof(sge_screens_data), MEMORY_TAG_WINDOW);
        if (screens_data == NULL) {
                return NULL;
        }

        screens_data->screens = NULL;
        screens_data->num_screens = 0;

        EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)screens_data);

        return screens_data;
}

void set_window_title(HWND hwnd, char *title, ...) {
        if (!hwnd) {
                log_internal_event(LOG_LEVEL_ERROR, "tried to update window title without valid hwnd");
                return;
        }
        char title_message[256];
        va_list args;
        va_start(args, title);
        vsnprintf(title_message, sizeof(title_message), title, args);
        va_end(args);
        SetWindowTextA(hwnd, title_message);

}

LRESULT CALLBACK wndProc(const HWND hwnd, const UINT uMsg, const WPARAM wparam, const LPARAM lparam) {
        sge_window *window = (sge_window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        int border_size = 3;
        switch (uMsg) { https://learn.microsoft.com/de-de/windows/win32/winmsg
                case WM_CLOSE:
                        is_window_open = 0;
                        DestroyWindow(hwnd);
                        log_internal_event(LOG_LEVEL_INFO, "Window got closed by close");
                        return 0;
                case WM_DESTROY:
                        is_window_open = 0;
                        PostQuitMessage(0);
                        log_internal_event(LOG_LEVEL_INFO, "Window got closed by destroy");
                        return 0;
                case WM_KEYDOWN: { //https://learn.microsoft.com/de-de/windows/win32/inputdev/
                        //printf("KEY DOWN: %llu\n", wparam);
                        //printf("PARAMS KEY DOWN %llu\n", lparam);
                        if (!is_tracking_enabled) {
                                break;
                        }
                        key_states[wparam] = SGE_TRUE;
                } break;
                case WM_KEYUP: {
                        //printf("KEY UP: %llu\n", wparam);
                        //printf("PARAMS KEY UP %llu\n", lparam);
                        if (!is_tracking_enabled) {
                                break;
                        }
                        key_states[wparam] = SGE_FALSE;
                } break;
                case WM_MOUSEMOVE: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        if (ignore_next_mousemove) {
                                ignore_next_mousemove = false;
                                break;
                        }
                        int x_pos = GET_X_LPARAM(lparam);
                        int y_pos = GET_Y_LPARAM(lparam);
                        //printf("Mouse move: x = %d, y = %d\n", x_pos, y_pos);
                        if (y_pos < -12000) { //experimental fix, sometimes Mouse move: x = 1, y = -16353, figured out if u move over border windows sucks
                                break;
                        }
                        POINT client_pos = {x_pos, y_pos};
                        ClientToScreen(hwnd, &client_pos);
                        x_pos = client_pos.x;
                        y_pos = client_pos.y;

                        if (!tracki_mousi) {
                                TRACKMOUSEEVENT leave_event = {
                                        .cbSize = sizeof(TRACKMOUSEEVENT),
                                        .dwFlags = TME_LEAVE,
                                        .hwndTrack = hwnd,
                                };
                                TrackMouseEvent(&leave_event);
                                tracki_mousi = SGE_TRUE;
                        }

                        //todo make smoother very laggy
                        if (is_hidden) {
                                //printf("NEW MOUSE POS\nX: %i, Y: %i\n\n", x_pos, y_pos);
                                //printf("LAST MOUSE POS\nX: %i, Y: %i\n\n", last_mouse_pos.x, last_mouse_pos.y);
                                mouse_pos center_mouse_pos = get_window_center(hwnd);
                                //printf("CENTER MOUS POS\nX: %i, Y: %i\n\n", center_mouse_pos.x, center_mouse_pos.y);
                                delta_mouse_pos.x = x_pos - last_mouse_pos.x;
                                delta_mouse_pos.y = y_pos - last_mouse_pos.y;
                                //printf("DELTA MOUSE POS\nX: %i, Y: %i\n\n", delta_mouse_pos.x, delta_mouse_pos.y);
                                last_mouse_pos.x = center_mouse_pos.x;
                                last_mouse_pos.y = center_mouse_pos.y;
                                SetCursorPos(last_mouse_pos.x, last_mouse_pos.y);
                                ignore_next_mousemove = true;
                        } else {
                                delta_mouse_pos.x = x_pos - last_mouse_pos.x;
                                delta_mouse_pos.y = y_pos - last_mouse_pos.y;
                                last_mouse_pos.x = x_pos;
                                last_mouse_pos.y = y_pos;
                        }


                } break;

                case WM_MOUSELEAVE: {
                        tracki_mousi = SGE_FALSE;
                        last_mouse_pos.x = -1;
                        last_mouse_pos.y = -1;
                        delta_mouse_pos.x = 0;
                        delta_mouse_pos.y = 0;
                }

                case WM_LBUTTONDOWN: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_LEFT] = SGE_TRUE;
                } break;
                case WM_LBUTTONUP: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_LEFT] = SGE_FALSE;
                } break;
                case WM_LBUTTONDBLCLK: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                } break;

                case WM_RBUTTONDOWN: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_RIGHT] = SGE_TRUE;
                } break;
                case WM_RBUTTONUP: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_RIGHT] = SGE_FALSE;
                } break;

                case WM_MBUTTONDOWN: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_MIDDLE] = SGE_TRUE;
                } break;
                case WM_MBUTTONUP: {
                        if (!is_tracking_enabled) {
                                break;
                        }
                        mouse_states[MBUTTON_MIDDLE] = SGE_FALSE;
                } break;

                case WM_XBUTTONDOWN: { //todo side buttons
                        if (!is_tracking_enabled) {
                                break;
                        }

                } break;

                case WM_NCHITTEST: {


                        if (!window) {
                                break;
                        }

                        if (!window->is_resizable) {
                                break;
                        }

                         if (window->current_window_mode != SGE_WINDOW_MODE_BORDERLESS) {
                                break;
                         }


                        int x_pos = GET_X_LPARAM(lparam);
                        int y_pos = GET_Y_LPARAM(lparam);

                        //TOP
                        if (y_pos <= window->usage_screen->top + window->y + border_size) {
                                if (x_pos <= window->usage_screen->left + window->x + border_size) {
                                        return HTTOPLEFT;
                                }
                                if (x_pos >= window->usage_screen->left + window->x + window->width - border_size) {
                                        return HTTOPRIGHT;
                                }
                                return HTTOP;
                        }

                        //BOTTOM
                        if (y_pos >= window->usage_screen->top + window->y + window->height - border_size) {
                                if (x_pos <= window->usage_screen->left + window->x + border_size) {
                                        return HTBOTTOMLEFT;
                                }
                                if (x_pos >= window->usage_screen->left + window->x + window->width - border_size) {
                                        return HTBOTTOMRIGHT;
                                }
                                return HTBOTTOM;
                        }

                        //LEFT
                        if (x_pos <= window->usage_screen->left + window->x + border_size) {
                                return HTLEFT;
                        }

                        //RIGHT
                        if (x_pos >= window->usage_screen->left + window->x + window->width - border_size) {
                                return HTRIGHT;
                        }


                        return HTCLIENT;
                }


                case WM_SIZE: {
                        //printf("RESIZE HAPPENED\n");
                        width = LOWORD(lparam);
                        height = HIWORD(lparam);
                        log_internal_event(LOG_LEVEL_INFO, "Resize happened: %d:%d", width, height);


                        //printf("width: %d, height: %d\n", width, height);
                        is_resize = true;
                        if (!window) {
                                break;
                        }
                        int x = 0;
                        int y = 0;

                        RECT rect;
                        if (GetWindowRect(hwnd, &rect)) {
                                x = rect.left;
                                y = rect.top;
                        }

                        window->x = x;
                        window->y = y;
                } break;
                case WM_SETCURSOR: {

                        POINT pt;
                        GetCursorPos(&pt);
                        int x_pos = pt.x;
                        int y_pos = pt.y;

                        if (y_pos <= window->usage_screen->top + window->y + border_size) {
                                break;
                        }

                        if (y_pos >= window->usage_screen->top + window->y + window->height - border_size) {
                                break;
                        }

                        if (x_pos <= window->usage_screen->left + window->x + border_size) {
                                break;
                        }

                        if (x_pos >= window->usage_screen->left + window->x + window->width - border_size) {
                                break;
                        }

                        SetCursor(LoadCursor(NULL, IDC_ARROW));
                        return 0;
                } break;
        }
        //printf("UMSG: %d\n", uMsg);
        return DefWindowProc(hwnd, uMsg, wparam, lparam);
}

sge_window *sge_window_create(const char *window_name, sge_window_create_settings *settings) {
        const HINSTANCE hInstance = GetModuleHandleA(NULL);

        const char CLASS_NAME[] = "Sample Window Class";
        if (!settings || !settings->width ||!settings->height) {
                log_internal_event(LOG_LEVEL_ERROR, "Wanted to create window without passing settings or widht/height");
                return NULL;
        }

        int width = settings->width;
        int height = settings->height;

        WNDCLASS wc = { };

        sge_screen_data *window_screen = allocate_memory(sizeof(sge_screen_data), MEMORY_TAG_WINDOW);
        if (settings->screen != NULL) {
                copy_memory(window_screen, settings->screen, sizeof(sge_screen_data), 0, 0);
        }

        wc.lpfnWndProc = wndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;

        if (RegisterClass(&wc) == 0) {
                const DWORD last_error = GetLastError();
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Failed to Register window Class. Error. %dw", last_error);
                log_internal_event(LOG_LEVEL_FATAL, error_message);
        }

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;

        DWORD dxExStyle = 0; //default
        DWORD dwStyle = WS_OVERLAPPEDWINDOW; //default


        SGE_BOOL found_monitor = SGE_FALSE;

        if (window_screen->width > 0 && window_screen->height > 0) {
                x = window_screen->left + settings->x;
                y = window_screen->top + settings->y;
                found_monitor = SGE_TRUE;
        }

        if (!found_monitor) {
                sge_screens_data *screens_data = get_screens_data();
                if (screens_data && screens_data->screens && screens_data->num_screens > 0) {
                        for (int i = 0; i < screens_data->num_screens; ++i) {
                                if (screens_data->screens[i].is_primary) {
                                        copy_memory(window_screen, &screens_data->screens[i], sizeof(sge_screen_data), 0, 0);

                                        log_internal_event(LOG_LEVEL_INFO, "Chose Primary Monitor as display: Width: %d, Height: %d", width, height);
                                        found_monitor = SGE_TRUE;
                                        break;
                                }
                        }
                        if (!found_monitor) {
                                copy_memory(window_screen, &screens_data->screens[0], sizeof(sge_screen_data), 0, 0);
                                width = window_screen->width;
                                height = window_screen->height;
                                log_internal_event(LOG_LEVEL_INFO, "Failed to find Primary Monitor choosing the first one with dimensions: Width: %d, Height: %d", width, height);
                        }
                }
        }

        if (settings->window_mode) {
                switch (settings->window_mode) {
                        case SGE_WINDOW_MODE_WINDOWED: {
                                if (settings->is_resizable) {
                                        dwStyle = WS_OVERLAPPEDWINDOW;
                                } else {
                                        dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
                                }
                        } break;
                        case SGE_WINDOW_MODE_BORDERLESS: {
                                dwStyle = WS_POPUP;
                        } break;
                        case SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN: {
                                dwStyle = WS_POPUP;
                                if (window_screen->width > 0 && window_screen->height > 0) {
                                        width = window_screen->width;
                                        height = window_screen->height;
                                } else {
                                        log_internal_event(LOG_LEVEL_FATAL, "no sreens data present");
                                        return NULL;
                                }
                        } break;
                        case SGE_WINDOW_MODE_TRUE_FULLSCREEN: {
                                // todo Needs more stuff for true fullscreen
                                dwStyle = WS_POPUP;


                        } break;
                        case SGE_WINDOW_MODE_WINDOWED_FULLSCREEN: {
                                dwStyle = WS_OVERLAPPEDWINDOW;
                                if (window_screen->width > 0 && window_screen->height > 0) {
                                        width = window_screen->width;
                                        height = window_screen->height;
                                } else {
                                        log_internal_event(LOG_LEVEL_FATAL, "no sreens data present");
                                        return NULL;
                                }
                        } break;
                }
        } else if (settings->is_resizable) {
                dwStyle = WS_OVERLAPPEDWINDOW;
        } else {
                dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }

        const HWND hwnd = CreateWindowEx(
                dxExStyle,
                CLASS_NAME,
                window_name,
                dwStyle, //https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
                x,
                y,
                width,
                height,
                NULL,
                NULL,
                hInstance,
                NULL
        );

        if (hwnd == NULL) {
                printf("hwnd is Null");
                const DWORD last_error = GetLastError();
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Failed to create Window. Error. %dw", last_error);
                log_internal_event(LOG_LEVEL_FATAL, error_message);
        }

        //https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow?redirectedfrom=MSDN
        if (settings->window_mode) {
                if (settings->window_mode == SGE_WINDOW_MODE_WINDOWED_FULLSCREEN) {
                        log_internal_event(LOG_LEVEL_INFO, "Showing maximized");
                        ShowWindow(hwnd,SW_SHOWMAXIMIZED);
                } else {
                        ShowWindow(hwnd, SW_SHOWDEFAULT);

                }
        } else {
                ShowWindow(hwnd, SW_SHOWDEFAULT);
        }



        log_internal_event(LOG_LEVEL_INFO, "Created Window");
        is_window_open = 1;

        sge_platform_handle handle = {
                .hwnd = hwnd,
                .hinstance = hInstance
        };

        RECT rect;
        if (GetWindowRect(hwnd, &rect)) {
                x = rect.left;
                y = rect.top;
                printf("Window position - X: %d, Y: %d\n", x, y);
        }

        sge_window *window = allocate_memory(sizeof(sge_window), MEMORY_TAG_WINDOW);
        if (window == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to allocate memory for window struct");
                return NULL;
        }
        window->handle = handle;
        window->height = height;
        window->width  = width;
        window->x      = x;
        window->y      = y;
        window->usage_screen = window_screen;
        window->is_resizable = settings->is_resizable == SGE_TRUE ? SGE_TRUE : SGE_FALSE;

        if (settings->window_mode) {
                window->current_window_mode = settings->window_mode;
        } else {
                window->current_window_mode = SGE_WINDOW_MODE_WINDOWED;
        }


        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

        return window;
}

SGE_BOOL sge_window_show(sge_window *window) {
        if (!ShowWindow(window->handle.hwnd, SW_SHOW)) {
                log_internal_event(LOG_LEVEL_ERROR, "Failed to Show Window");
                return SGE_FALSE;
        }
        log_internal_event(LOG_LEVEL_INFO, "Showed Window");
        is_window_open = 1;
        return SGE_TRUE;
}

SGE_BOOL sge_window_hide(sge_window *window) {
        if (!ShowWindow(window->handle.hwnd, SW_HIDE)) {
                log_internal_event(LOG_LEVEL_ERROR, "Failed to hide Window");
                return SGE_FALSE;
        }
        log_internal_event(LOG_LEVEL_INFO, "Showed Window");
        is_window_open = 0;
        return SGE_TRUE;
}

void hide_mouse() {
        if (is_hidden) {
                return;
        }
        last_visible_pos.x = last_mouse_pos.x;
        last_visible_pos.y = last_mouse_pos.y;

        printf("LAST VISIBLE: X: %i, Y :%i\n", last_visible_pos.x, last_visible_pos.y);

        is_hidden = true;
        ShowCursor(FALSE);
}



void show_mouse() {
        printf("LAST POS X: %i, Y: %i\n", last_visible_pos.x, last_visible_pos.y);
        is_hidden = false;
        ignore_next_mousemove = true;
        ShowCursor(TRUE);
        last_mouse_pos.x = last_visible_pos.x;
        last_mouse_pos.y = last_visible_pos.y;
        SetCursorPos(last_visible_pos.x, last_visible_pos.y);
}

void update_frame(const int target_fps, DWORD frame_start_ms, sge_window *window) {
        if (!frame_start_ms) {
                log_internal_event(LOG_LEVEL_ERROR, "no start time passed, setting to now -20");
                frame_start_ms = get_current_ms_time();
                frame_start_ms -= 20;
        }
        //printf("New Frame\n");
        delta_mouse_pos.x = 0;
        delta_mouse_pos.y = 0;
        if (is_tracking_enabled) {
                update_key_states();
        }

        if (height && height != window->height) {
                window->height = height;
        }
        if (width && width != window->width) {
                window->width = width;
        }

        MSG msg;
        while (PeekMessage(&msg, NULL, 0 ,0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        if (g_delta_time > 1000) {
                g_delta_time = 0;
                //printf("FPS: %d\n", g_fps);
                set_window_title(window->handle.hwnd, "Running with %d Fps", g_fps);
                g_fps = 0;
        } else {
                g_fps++;
        }



        if (target_fps > 0) {
                const DWORD target_frame_time = 1000 / target_fps;
                //printf("TARGET FRAME TIME: %d\n", target_frame_time);
                double frame_time;
                do  {
                        os_sleep(1);
                        frame_time = get_current_ms_time() - frame_start_ms;
                } while (frame_time < target_frame_time);
                //printf("FRAME TIME: %f\n", frame_time);
        }

        const DWORD frame_end_time = get_current_ms_time();
        g_delta_time += frame_end_time - frame_start_ms;
}

SGE_RESULT sge_window_mode_update(sge_window *window, sge_window_create_settings *settings) {
        if (!settings || !window) {
                return SGE_INVALID_API_CALL;
        }

        if (!settings->window_mode) {
                return SGE_INVALID_API_CALL;
        }

        DWORD dwStyle = WS_OVERLAPPEDWINDOW;
        int x = window->x;
        int y = window->y;
        int width = window->height;
        int height = window->height;

        sge_screen_data *window_screen = allocate_memory(sizeof(sge_screen_data), MEMORY_TAG_WINDOW);
        if (!window_screen) {
                allocation_error();
                return SGE_ERROR_FAILED_ALLOCATION;
        }
        if (settings->screen != NULL) {
                copy_memory(window_screen, settings->screen, sizeof(sge_screen_data), 0, 0);
        }

        if (settings->width) {
                width = settings->width;
        }
        if (settings->height) {
                height = settings->height;
        }

        SGE_BOOL found_monitor = SGE_FALSE;

        if (window_screen->width > 0 && window_screen->height > 0) {
                x = window_screen->left + settings->x;
                y = window_screen->top + settings->y;
                found_monitor = SGE_TRUE;
        }

        if (!found_monitor) {
                sge_screens_data *screens_data = get_screens_data();
                if (screens_data && screens_data->screens && screens_data->num_screens > 0) {
                        for (int i = 0; i < screens_data->num_screens; ++i) {
                                if (screens_data->screens[i].is_primary) {
                                        copy_memory(window_screen, &screens_data->screens[i], sizeof(sge_screen_data), 0, 0);

                                        log_internal_event(LOG_LEVEL_INFO, "Chose Primary Monitor as display: Width: %d, Height: %d", width, height);
                                        found_monitor = SGE_TRUE;
                                        break;
                                }
                        }
                        if (!found_monitor) {
                                copy_memory(window_screen, &screens_data->screens[0], sizeof(sge_screen_data), 0, 0);
                                width = window_screen->width;
                                height = window_screen->height;
                                log_internal_event(LOG_LEVEL_INFO, "Failed to find Primary Monitor choosing the first one with dimensions: Width: %d, Height: %d", width, height);
                        }
                }
        }

        switch (settings->window_mode) {
                case SGE_WINDOW_MODE_WINDOWED: {
                        dwStyle = WS_OVERLAPPEDWINDOW;

                } break;
                case SGE_WINDOW_MODE_BORDERLESS: {
                        dwStyle = WS_POPUP;
                } break;
                case SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN: {
                        dwStyle = WS_POPUP;
                        if (window_screen->width > 0 && window_screen->height > 0) {
                                width = window_screen->width;
                                height = window_screen->height;
                                x = window_screen->left;
                                y = window_screen->top;
                        } else {
                                log_event(LOG_LEVEL_ERROR, "FAiled to find screend Data");
                                return SGE_ERROR;
                        }
                } break;
                case SGE_WINDOW_MODE_TRUE_FULLSCREEN: {
                        // todo Needs more stuff for true fullscreen
                        dwStyle = WS_POPUP;


                } break;
                case SGE_WINDOW_MODE_WINDOWED_FULLSCREEN: {
                        dwStyle = WS_OVERLAPPEDWINDOW;
                        if (window_screen->width > 0 && window_screen->height > 0) {
                                width = window_screen->width;
                                height = window_screen->height;
                                x = window_screen->left;
                                y = window_screen->top;
                        } else {
                                log_internal_event(LOG_LEVEL_FATAL, "no sreens data present");
                                return SGE_ERROR;
                        }
                } break;
        }

        SetWindowLong(window->handle.hwnd, GWL_STYLE, dwStyle);
        if (!SetWindowPos(window->handle.hwnd, 0,
                 x, y, width, height,
                 SWP_FRAMECHANGED)) {
                printf("ERROR");
                return SGE_ERROR;
        }
        ShowWindow(window->handle.hwnd, SW_SHOW);


        window->current_window_mode = settings->window_mode;
        window->usage_screen = window_screen;

        return SGE_SUCCESS;
}

SGE_BOOL window_should_close() {
        if (is_window_open) {
                return SGE_FALSE;
        }
        return SGE_TRUE;
}

SGE_RESULT sge_window_destroy(sge_window *window) {
        if (!window) {
                return SGE_INVALID_API_CALL;
        }
        printf("DESTROYYYYYY RAWRRRRRRR\n");
        if (!DestroyWindow(window->handle.hwnd)) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}

#endif




//HWND hwnd = CreateWindowEx(
//0,                              // Optional window styles.
//CLASS_NAME,                     // Window class
//L"Learn to Program Windows",    // Window text
//WS_OVERLAPPEDWINDOW,            // Window style
//
//// Size and position
//CW_USEDEFAULT,
//CW_USEDEFAULT,
//CW_USEDEFAULT,
//CW_USEDEFAULT,
//
//NULL,       // Parent window
//NULL,       // Menu
//hInstance,  // Instance handle
//NULL        // Additional application data
//);
//
//if (hwnd == NULL)
//{
//        return 0;
//}