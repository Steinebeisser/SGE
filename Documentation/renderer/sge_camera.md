# SGE Camera Module

## Overview

The SGE Camera Module allows to modify the camera in a render instance for  specific regions

## Table of Content

1. [Structures](#structures)
   1.1 [sge_camera](#sge_camera) <br>
   1.2 [sge_uniform_buffer_object](#sge_uniform_buffer_object) <br>
   1.3 [sge_movement_settings](#sge_movement_settings) <br>
   1.4 [sge_mouse_movement_settings](#sge_mouse_movement_settings) <br>
2. [Enumerations](#enumerations)
   2.1 [MOVEMENT_MODE](#movement_mode) <br>
3. [Defines](#defines)
   3.1 [SGE_MOUSE_INVERT_X](#sge_mouse_invert_x) <br>
   3.2 [SGE_MOUSE_INVERT_Y](#sge_mouse_invert_y) <br>
4. [API Reference](#api-reference)
   4.1 [sge_move](#sge_result-sge_move) <br>
   4.2 [sge_camera_move_forward](#sge_result-sge_camera_move_forward) <br>
   4.3 [sge_camera_move_left](#sge_result-sge_camera_move_left) <br>
   4.4 [sge_camera_move_right](#sge_result-sge_camera_move_right) <br>
   4.5 [sge_camera_move_backwards](#sge_result-sge_camera_move_backwards) <br>
   4.6 [sge_camera_move_down](#sge_result-sge_camera_move_down) <br>
   4.7 [sge_camera_move_up](#sge_result-sge_camera_move_up) <br>
   4.8 [sge_camera_rotate](#sge_result-sge_camera_rotate) <br>
   4.9 [sge_camera_rotate_x](#sge_result-sge_camera_rotate_x) <br>
   4.10 [sge_camera_rotate_y](#sge_result-sge_camera_rotate_y) <br>
   4.11 [sge_camera_rotate_z](#sge_result-sge_camera_rotate_z) <br>
5. [Important Notice](#important-notice)
6. [Examples](#examples)

## Structures

### `sge_camera`
```c
typedef struct sge_camera {
        vec3 position; //x,   y,     z
        vec3 rotation; //yaw, pitch, roll
        float fov;
        float speed;
} sge_camera;
```

---

### `sge_uniform_buffer_object`
```c
typedef struct sge_uniform_buffer_object {
        m4      model;
        m4      view;
        m4      proj;
} sge_uniform_buffer_object;
```

---

### `sge_movement_settings`
```c
typedef struct sge_movement_settings {
        MOVEMENT_MODE   mode;
        float           delta_speed;
} sge_movement_settings;
```

---

### `sge_mouse_movement_settings`
```c
typedef struct sge_mouse_movement_settings {
        float           mouse_delta_x;
        float           mouse_delta_y;
        float           sensitivity;
        unsigned int    flags;
} sge_mouse_movement_settings;
```

## Enumerations

### `MOVEMENT_MODE`
```c
typedef enum {
        ROTATE_YAW_ONLY,
        ROTATE_PITCH_ONLY,
        ROTATE_PITCH_AND_YAW,
        ROTATE_NONE
} MOVEMENT_MODE;
```

## Defines

### `SGE_MOUSE_INVERT_X`
```c
#define SGE_MOUSE_INVERT_X (1<<0)
```

---

### `SGE_MOUSE_INVERT_Y`
```c
#define SGE_MOUSE_INVERT_Y (1<<1)
```

---

## API Reference

### `SGE_RESULT sge_move`
```c
SGE_RESULT sge_move(sge_render *render, sge_region *move_region, vec3 direction, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where the movement is applied
- `direction`: A normalized `vec3` defining movement direction
- `movement_settings`: `sge_movement_settings` struct

#### Returns:
- `SGE_SUCCESS`: on success
- `SGE_INVALID_API_CALL`: if any parameter is null or invalid

---

### `SGE_RESULT sge_camera_move_forward`
```c
SGE_RESULT sge_camera_move_forward(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies
- `movement_settings`: Contains movement speed and mode

#### Returns:
- Same as `sge_move`

---

### `SGE_RESULT sge_camera_move_left`
```c
SGE_RESULT sge_camera_move_left(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies
- `movement_settings`: Contains movement speed and mode

#### Returns:
- Same as `sge_move`

---

### `SGE_RESULT sge_camera_move_right`
```c
SGE_RESULT sge_camera_move_right(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies
- `movement_settings`: Contains movement speed and mode

#### Returns:
- Same as `sge_move`

---

### `SGE_RESULT sge_camera_move_backwards`
```c
SGE_RESULT sge_camera_move_backwards(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies.
- `movement_settings`: Contains movement speed and mode.

#### Returns:
- Same as `sge_move`.

---

### `SGE_RESULT sge_camera_move_down`
```c
SGE_RESULT sge_camera_move_down(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies.
- `movement_settings`: Contains movement speed and mode.

#### Returns:
- Same as `sge_move`.

---

### `SGE_RESULT sge_camera_move_up`
```c
SGE_RESULT sge_camera_move_up(sge_render *render, sge_region *move_region, sge_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where movement applies.
- `movement_settings`: Contains movement speed and mode.

#### Returns:
- Same as `sge_move`.

---

### `SGE_RESULT sge_camera_rotate`
```c
SGE_RESULT sge_camera_rotate(sge_render *render, sge_region *move_region, sge_mouse_movement_settings movement_settings);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where rotation applies
- `movement_settings`: Contains mouse movement deltas and sensitivity

#### Returns:
- `SGE_SUCCESS` on success
- `SGE_INVALID_API_CALL` if parameters are invalid

---

### `SGE_RESULT sge_camera_rotate_x`
```c
SGE_RESULT sge_camera_rotate_x(sge_render *render, sge_region *move_region, float angle);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where rotation applies
- `angle`: The amount of rotation in degrees

#### Returns:
- `SGE_SUCCESS` on success
- `SGE_INVALID_API_CALL` if parameters are invalid

---

### `SGE_RESULT sge_camera_rotate_y`
```c
SGE_RESULT sge_camera_rotate_y(sge_render *render, sge_region *move_region, float angle);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where rotation applies
- `angle`: The amount of rotation in degrees

#### Returns:
- Same as `sge_camera_rotate_x`.

---

### `SGE_RESULT sge_camera_rotate_z`
```c
SGE_RESULT sge_camera_rotate_z(sge_render *render, sge_region *move_region, float angle);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `move_region`: The region where rotation applies
- `angle`: The amount of rotation in degrees

#### Returns:
- Same as `sge_camera_rotate_x`.

## Important Notice

### Uniform Buffer Update
The uniform buffer is updated during the call to `sge_draw_frame(...)`.

## Examples

### Creating movement settings

```c
sge_movement_settings movement_settings_horizontal = {
      .mode = ROTATE_PITCH_AND_YAW,
      .delta_speed = 0.5f,
};
sge_movement_settings left_right = {
      .mode = ROTATE_YAW_ONLY,
      .delta_speed = 0.5f,
};
sge_movement_settings sprint_settings_horizontal = {
      .mode = ROTATE_PITCH_AND_YAW,
      .delta_speed = 3.0f,
};
sge_movement_settings movement_settings_vertical = { 
      .mode = ROTATE_NONE, 
      .delta_speed = 0.2f
}

update per loop:
    mouse_pos delta_mouse_pos = get_delta_mouse_position();
    
    sge_mouse_movement_settings mouse_movement_settings = {
          .mouse_delta_x = delta_mouse_pos.x,
          .mouse_delta_y = delta_mouse_pos.y,
          .sensitivity = 1,
          .flags = 0,
    };
```

### Moving the camera

```c
if (is_key_down(KEY_A)) {
      sge_camera_move_left(render, active_region, left_right);
}

if (is_key_down(KEY_W)) {
        if (is_key_down(KEY_CTRL)) {
                sge_camera_move_forward(render, active_region, sprint_settings_horizontal);
        } else {
                sge_camera_move_forward(render, active_region, movement_settings_horizontal);
        }
}

if (is_key_down(KEY_S)) {
         sge_camera_move_backwards(render, active_region, movement_settings_horizontal);
}

if (is_key_down(KEY_D)) {
         sge_camera_move_right(render, active_region, left_right);
}

if (is_key_down(KEY_SPACE)) {
         sge_camera_move_up(render, active_region, movement_settings_vertical);
}

if (is_key_down(KEY_SHIFT)) {
         sge_camera_move_down(render, active_region, movement_settings_vertical);
}
```

### Rotating

```c
if (is_key_pressed(KEY_ARROW_LEFT)) {
         sge_camera_rotate_y(render,active_region, -15);
}

if (is_key_pressed(KEY_ARROW_RIGHT)) {
         sge_camera_rotate_y(render, active_region, 15);
}

if (is_key_pressed(KEY_ARROW_UP)) {
         sge_camera_rotate_x(render, active_region, 15);
}

if (is_key_pressed(KEY_ARROW_DOWN)) {
         sge_camera_rotate_x(render, active_region, -15);
}

if (is_mouse_down(MBUTTON_RIGHT)) {
         hide_mouse();
         sge_camera_rotate(render, active_region, mouse_movement_settings);
}
```




