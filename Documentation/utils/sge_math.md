# SGE Math Module

## Overview

The SGE Math Module is the Math interface in the SGE library to fulfill all needs for Game Creation

## Table of Content

1. [Structures](#structures) <br>
   1.1 [vec3](#vec3) <br>
   1.2 [vec4](#vec4) <br>
   1.3 [m4](#m4) <br>
2. [Constants](#constants) <br>
   2.1 [crc32_table](#crc32_table)
3. [API Reference]() <br>
   3.1 [sge_m4_set_identity](#void-sge_m4_set_identity) <br>
   3.2 [sge_m4_set_value](#void-sge_m4_set_value) <br>
   3.3 [sge_m4_get_value](#float-sge_m4_get_value) <br>
   3.4 [sge_m4_set_translate](#void-sge_m4_set_translate) <br>
   3.5 [sge_m4_set_scale](#void-sge_m4_set_scale) <br>
   3.6 [sge_m4_set_rotate_x](#void-sge_m4_set_rotate_x) <br>
   3.7 [sge_m4_set_rotate_y](#void-sge_m4_set_rotate_y) <br>
   3.8 [sge_m4_set_rotate_z](#void-sge_m4_set_rotate_z) <br>
   3.9 [sge_m4_multiply](#void-sge_m4_multiply) <br>
   3.10 [sge_m4_transpose](#void-sge_m4_transpose) <br>
   3.11 [sge_m4_set_rotate](#void-sge_m4_set_rotate) <br>
   3.12 [sge_m4_copy](#void-sge_m4_copy) <br>
   3.13 [sge_m4_print](#void-sge_m4_print) <br>
   3.14 [sge_vec4_print](#void-sge_vec4_print) <br>
   3.15 [sge_vec3_print](#void-sge_vec3_print) <br>
   3.16 [sge_m4_transform_vec4](#vec4-sge_m4_transform_vec4) <br>
   3.17 [squared](#size_t-squared) <br>
4. [Examples](#examples) <br>
   4.1 [Adding 2 Vec3 Vectors](#adding-two-vec3-vectors) <br>
   4.2 [Normalizing a Vec3](#normalizing-a-vec3) <br>
   4.3 [Cross Product of Two vec3](#cross-product-of-two-vec3) <br>
   4.4 [Creating a Translation Matrix](#creating-a-translation-matrix) <br>
   4.5 [Transforming a vec4 with a Matrix](#transforming-a-vec4-with-a-matrix) <br>
   4.6 [Calculating CRC32 Checksum](#calculating-crc32-checksum) <br>

## Structures

### Vec3

```c
typedef struct vec3 {
        float x;
        float y;
        float z;
} vec3;
```

### Vec4
```c
typedef struct vec4 {
        float x;
        float y;
        float z;
        float a;
} vec4;
```

### M4
```c
typedef float m4[4][4];
```

**Row Major**

## Constants

### CRC32_Table
```c
extern uint32_t crc32_table[256];
```

## API Reference


### vec3 sge_vec3_add(...)

```c
vec3 sge_vec3_add(vec3 a, vec3 b);
```

#### Parameters:
- `a`: vec3 to add to
- `b`: vec3 to be added

#### Returns:
- `vec3`: resulting vec3 from `a + b` 

---

### vec3 sge_vec3_sub(...)

```c
vec3 sge_vec3_sub(vec3 a, vec3 b);
```

#### Parameters:
- `a`: vec3 to subtract from
- `b`: vec3 to be subtracted

#### Returns:
- `vec3`: resulting vec3 from `a - b`

---

### vec3 sge_vec3_normalize(...)

```c
vec3 sge_vec3_normalize(vec3 vector);
```

#### Parameters:
- `vector`: vector to be normalized

#### Returns:
- `vec3`: direction vector 

---

### vec3 sge_vec3_cross(...)

```c
vec3 sge_vec3_cross(vec3 vector1, vec3 vector2);
```

#### Parameters:
- `vector1`: First input vector
- `vector2`: Second input vector

#### Returns:
- `vec3`: Cross product of `vector1` and `vector2`

---

### vec3 sge_vec3_scale(...)

```c
vec3 sge_vec3_scale(vec3 vector1, float scale_factor);
```

#### Parameters:
- `vector1`: Vector to scale
- `scale_factor`: Amount to scale each component by

#### Returns:
- `vec3`: Scaled vector

---

### vec3 sge_vec3_rotate_yaw_pitch(...)

```c
vec3 sge_vec3_rotate_yaw_pitch(vec3 vector1, vec3 rotation);
```

#### Parameters:
- `vector1`: Input vector to rotate
- `rotation`: Yaw (y-axis) and pitch (x-axis) angles in degrees

#### Returns:
- `vec3`: Rotated vector

---

### vec3 sge_vec3_rotate_pitch(...)

```c
vec3 sge_vec3_rotate_pitch(vec3 vector, vec3 rotation);
```

#### Parameters:
- `vector`: Input vector
- `rotation`: pitch (x-axis) angles in degrees

#### Returns:
- `vec3`: Rotated vector

---

### vec3 sge_vec3_rotate_yaw(...)

```c
vec3 sge_vec3_rotate_yaw(vec3 vector1, vec3 rotation);
```

#### Parameters:
- `vector1`: Input vector
- `rotation`: yaw (x-axis) angles in degrees

#### Returns:
- `vec3`: Rotated vector

---

### void sge_m4_set_identity(...)

```c
void sge_m4_set_identity(m4 matrix);
```

#### Parameters:
- `matrix`: Matrix to set to identity.

#### Returns:
- `void`

---

### void sge_m4_set_value(...)

```c
void sge_m4_set_value(m4 matrix, int row, int col, float value);
```

#### Parameters:
- `matrix`: Matrix to modify
- `row`: Row index (0-3)
- `col`: Column index (0-3)
- `value`: Value to assign

#### Returns:
- `void`

---

### float sge_m4_get_value(...)

```c
float sge_m4_get_value(m4 matrix, int row, int col);
```

#### Parameters:
- `matrix`: Matrix to read from
- `row`: Row index (0-3)
- `col`: Column index (0-3)

#### Returns:
- `float`: Value at specified row and column

---

### void sge_m4_set_translate(...)

```c
void sge_m4_set_translate(m4 matrix, vec3 translate);
```

#### Parameters:
- `matrix`: Matrix to modify
- `translate`: Translation vector

#### Returns:
- `void`

---

### void sge_m4_set_scale(...)

```c
void sge_m4_set_scale(m4 matrix, vec3 scale);
```

#### Parameters:
- `matrix`: Matrix to modify
- `scale`: Scale vector (x, y, z)

#### Returns:
- `void`

---

### void sge_m4_set_rotate_x(...)

```c
void sge_m4_set_rotate_x(m4 matrix, float angle);
```

#### Parameters:
- `matrix`: Matrix to apply rotation
- `angle`: Rotation angle around the X-axis (in radians or degrees)

#### Returns:
- `void`

---

### void sge_m4_set_rotate_y(...)

```c
void sge_m4_set_rotate_y(m4 matrix, float angle);
```

#### Parameters:
- `matrix`: Matrix to apply rotation
- `angle`: Rotation angle around the Y-axis

#### Returns:
- `void`

---

### void sge_m4_set_rotate_z(...)

```c
void sge_m4_set_rotate_z(m4 matrix, float angle);
```

#### Parameters:
- `matrix`: Matrix to apply rotation
- `angle`: Rotation angle around the Z-axis

#### Returns:
- `void`

---

### void sge_m4_multiply(...)

```c
void sge_m4_multiply(m4 m_result, m4 m_a, m4 m_b);
```

#### Parameters:
- `m_result`: Output matrix to store result
- `m_a`: First matrix
- `m_b`: Second matrix

#### Returns:
- `void`

---

### void sge_m4_transpose(...)

```c
void sge_m4_transpose(m4 matrix);
```

#### Parameters:
- `matrix`: Matrix to transpose

#### Returns:
- `void`

---

### void sge_m4_set_rotate(...)

```c
void sge_m4_set_rotate(m4 matrix, vec3 rotation);
```

#### Parameters:
- `matrix`: Matrix to apply rotation
- `rotation`: Rotation vector (pitch, yaw, roll)

#### Returns:
- `void`

---

### void sge_m4_copy(...)

```c
void sge_m4_copy(m4 m_to_copy, m4 m_from_copy);
```

#### Parameters:
- `m_to_copy`: matrix to copy to
- `m_from_copy`: matrix to copy from

#### Returns:
- `void`

---

### void sge_m4_print(...)

```c
void sge_m4_print(m4 matrix);
```

#### Parameters:
- `matrix`: Matrix to print (to stdout)

#### Returns:
- `void`

---

### void sge_vec4_print(...)

```c
void sge_vec4_print(vec4 vector);
```

#### Parameters:
- `vector`: vec4 to print (to stdout)

#### Returns:
- `void`:

---

### void sge_vec3_print(...)

```c
void sge_vec3_print(vec3 vector);
```

#### Parameters:
- `vector`: vec3 to print (to stdout)

#### Returns:
- `void`:

---


### vec4 sge_m4_transform_vec4(...)

```c
vec4 sge_m4_transform_vec4(m4 matrix, vec4 vec);
```

#### Parameters:
- `matrix`: Transformation matrix
- `vec`: Input 4D vector

#### Returns:
- `vec4`: Transformed vector

---

### size_t squared(...)

```c
size_t squared(size_t num);
```

#### Parameters:
- `num`: Input number

#### Returns:
- `size_t`: Square of the input number (`num * num`)

---

## Examples

### Adding Two vec3 Vectors

```c
vec3 a = {1.0f, 2.0f, 3.0f};
vec3 b = {4.0f, 5.0f, 6.0f};
vec3 result = sge_vec3_add(a, b);
printf("Result: (%f, %f, %f)\n", result.x, result.y, result.z);
```

**Output:**
```
Result: (5.000000, 7.000000, 9.000000)
```

---

### Normalizing a vec3

```c
vec3 v = {3.0f, 4.0f, 0.0f};
vec3 normalized = sge_vec3_normalize(v);
printf("Normalized: (%f, %f, %f)\n", normalized.x, normalized.y, normalized.z);
```

**Output:**
```
Normalized: (0.600000, 0.800000, 0.000000)
```

---

### Cross Product of Two vec3

```c
vec3 a = {1.0f, 0.0f, 0.0f};
vec3 b = {0.0f, 1.0f, 0.0f};
vec3 cross = sge_vec3_cross(a, b);
printf("Cross product: (%f, %f, %f)\n", cross.x, cross.y, cross.z);
```

**Output:**
```
Cross product: (0.000000, 0.000000, 1.000000)
```

---

### Creating a Translation Matrix

```c
vec3 translation = {2.0f, 3.0f, 4.0f};
m4 matrix;
sge_m4_set_identity(matrix);
sge_m4_set_translate(matrix, translation);
sge_m4_print(matrix);
```

**Output:**
```
[1 0 0 2]
[0 1 0 3]
[0 0 1 4]
[0 0 0 1]
```

---

### Transforming a vec4 with a Matrix

```c
vec4 point = {1.0f, 2.0f, 3.0f, 1.0f};
m4 matrix;
sge_m4_set_identity(matrix);
sge_m4_set_translate(matrix, (vec3){5.0f, 0.0f, 0.0f});
vec4 result = sge_m4_transform_vec4(matrix, point);
printf("Transformed point: (%f, %f, %f, %f)\n", result.x, result.y, result.z, result.a);
```

**Output:**
```
Transformed point: (6.000000, 2.000000, 3.000000, 1.000000)
```

### Calculating CRC32 Checksum

```c
const char *test_string = "hello world";
size_t length = strlen(test_string);

uint32_t crc = 0xFFFFFFFF;
uint8_t *bytes = (uint8_t *)test_string;

for (size_t i = 0; i < length; i++) {
  uint8_t byte = bytes[i];
  crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ byte];
}
crc = crc ^ 0xFFFFFFFF;

printf("CRC32 of \"%s\" = 0x%08X\n", test_string, crc);
```

**Output:**

```c
CRC32 of "hello world" = 0x0D4A1185
```
