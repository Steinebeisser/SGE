# SGE Window Module

## Overview

The SGE Window Module creates a "Platform Independent" (currently only Windows) interface to create/manage windows for SGE.

## Structures

### `sge_screen_data`
Represents a display screen.
```c
typedef struct sge_screen_data {
        int     width;
        int     height;
        char    name[64];
        bool    is_primary;
        int     left;
        int     right;
        int     bottom;
        int     top;
} sge_screen_data;
```

### `sge_screens_data`
Represents a collection of screens.
```c
typedef struct sge_screens_data {
        sge_screen_data     *screens;
        int                 num_screens;
} sge_screens_data;
```

### `sge_platform_handle`
Holds data for accessing/managing the window based on the operating system.
```c
#ifdef WIN32
typedef struct sge_platform_handle {
        HWND hwnd;              // Window handle
        HINSTANCE hinstance;    // Application instance
} sge_platform_handle;
#elif UNIX
typedef struct sge_platform_handle {
        // Whatever Unix needs
} sge_platform_handle;
#else

#endif
```

### `sge_window`
Window structure needed to perform window operations.
```c
typedef struct sge_window {
        sge_platform_handle handle;
        int                 width;
        int                 height;
        int                 x;
        int                 y;
        SGE_WINDOW_MODE     current_window_mode;
        sge_screen_data     *usage_screen;
        SGE_BOOL            is_resizable;
} sge_window;
```

### `sge_window_create_settings`
Defines settings used when creating a window.
```c
typedef struct sge_window_create_settings {
        SGE_WINDOW_MODE     window_mode;
        SGE_BOOL            is_resizable;
        int                 width;
        int                 height;
        int                 x;
        int                 y;
        sge_screen_data     *screen; // if you want to display on different screen than primary
} sge_window_create_settings;
```

## Enumerations

### `SGE_WINDOW_MODE`
```c
typedef enum SGE_WINDOW_MODE {
        SGE_WINDOW_MODE_WINDOWED,
        SGE_WINDOW_MODE_BORDERLESS,
        SGE_WINDOW_MODE_TRUE_FULLSCREEN, // Doesnt do anything yet
        SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN,
        SGE_WINDOW_MODE_WINDOWED_FULLSCREEN,
} SGE_WINDOW_MODE;
```

## API Reference

### `sge_window *sge_window_create(const char *window_name, sge_window_create_settings *settings);`

#### Parameters:
- `window_name`: Pointer to a string for the window name.
- `settings`: Pointer to a `sge_window_create_settings` struct containing window configuration.

#### Result:
- Pointer to the created `sge_window` struct.
- `NULL` if it failed to create the window.

### Example
```c

```
---

### `SGE_RESULT sge_window_destroy(sge_window *window);`

#### Parameters:
- `window`: Pointer to sge_window struct which must be created using `sge_window_create`

#### Returns:
- `SGE_INVALID_API_CALL` if no window was passed or the handle is incorrect
- `SGE_ERROR` if the OS failed to destroy the window
- `SGE_SUCCESS` if the window got destroyed

---

### `SGE_BOOL sge_window_show(sge_window *window);`

#### Parameters:
- `window`: Pointer to the window to be shown.

#### Returns:
- `SGE_TRUE` if the window was successfully shown.
- `SGE_FALSE` if the operation failed.

---

### `SGE_BOOL sge_window_hide(sge_window *window);`

#### Parameters:
- `window`: Pointer to the window to be hidden.

#### Returns:
- `SGE_TRUE` if the window was successfully hidden.
- `SGE_FALSE` if the operation failed.

---

### `void update_frame(int target_fps, DWORD start_time_ms, sge_window *window);`
Gets all Window Messages like Button Clicks and Mouse Pos, handles every Window Specific Task Like Resizes and so on

#### Parameters:
- `target_fps`: Desired frames per second.
- `start_time_ms`: Time at which the frame update started.
- `window`: Pointer to the window being updated.

---

### `SGE_RESULT sge_window_mode_update(sge_window *window, sge_window_create_settings *settings);`
Updates the Window mode, Size, Position, Monitor

#### Parameters:
- `window`: Pointer to the window whose mode should be updated.
- `settings`: Pointer to the new settings.

#### Returns:
- `SGE_SUCCESS` if the window mode was successfully updated.
- `SGE_ERROR` if the operation failed.

---

### `void hide_mouse();`
Hides the mouse and centers it in the middle of the window so you can have infinite movement

---

### `void show_mouse();`
Shows the Mouse again in the last visible position before it was being hidden

---

### `SGE_BOOL window_should_close();`

#### Returns:
- `SGE_TRUE` if the window should close.
- `SGE_FALSE` if not.

---

### `mouse_pos screen_to_window(sge_render *render, mouse_pos screen_pos);`

#### Parameters:
- `render`: Pointer to the render struct `sge_render`.
- `screen_pos`: The screen position to convert.

#### Returns:
- The corresponding window-relative position.

---

### `sge_screens_data *get_screens_data();`

#### Returns:
- Pointer to an `sge_screens_data` structure containing screen information.

---

### Examples

**Scan Screens and copy the last non-primary screen into `secondary_screen`**
```c
sge_screens_data *screen_data = get_screens_data();
sge_screen_data *secondary_screen = allocate_memory(sizeof(sge_screen_data), MEMORY_TAG_WINDOW);
for (int i = 0; i < screen_data->num_screens; ++i) {
        sge_screen_data screen = screen_data->screens[i];
        if (!screen.is_primary) {
                copy_memory(secondary_screen, &screen, sizeof(sge_screen_data), 0, 0);
        }
}
```

**Create a Window in Borderless Mode with Size 700x500 that's resizable**
```c
sge_window_create_settings window_create_settings = {
        .window_mode = SGE_WINDOW_MODE_BORDERLESS,
        .width = 700,
        .height = 500,
        .is_resizable = SGE_TRUE
};
sge_window *window= sge_window_create("Test Window", &window_create_settings);
```

**Terminate if no secondary screen else update window to be borderless fullscreen on the secondary Monitor, Width Height x and y will be ignored because its Fullscreen, its also not resizeable**
```c
if (!secondary_screen) {
        terminate_program();
}
sge_window_create_settings win_update_settings = {
        .window_mode = SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN,
        .screen = secondary_screen,
        .x = 100,
        .y = 200,
        .height = 500,
        .width = 2000,
        .is_resizable = SGE_FALSE
};
sge_window_mode_update(window, &win_update_settings);
```

**Is Running while not pressing Left Mouse Button, if Right Mouse Button is pressed down it hides the mouse and 
centers in window screen, if it was down last frame(not pressed anymore) shows it in its original position<br> 
If not running anymore destroying Window**
```c
SGE_BOOL is_running = SGE_TRUE;
while (is_running) {
        if (is_mouse_down(MBUTTON_RIGHT)) { //function in core/input.h
                hide_mouse();
        }
        if (was_mouse_down(MBUTTON_RIGHT)) {
                show_mouse();
        }
        if (is_mouse_pressed(MBUTTON_LEFT)) {
                is_running = SGE_FALSE;
        }
        update_frame(target_fps, start_time, window);
}

sge_window_destroy(window);
```