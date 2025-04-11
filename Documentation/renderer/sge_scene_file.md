# SGE SCENE FILE Module

## Overview

The SGE Scene File Module is used to create a load scenes to apply transformations to renderables and combine multiple in one

## Table of Content

1. [Structures](#structures) <br>
   1.1 [sge_scene_extension](#sge_scene_extension) <br>
   1.2 [sge_scene_header](#sge_scene_header) <br>
   1.3 [sge_scene_sgerend_section](#sge_scene_sgerend_section) <br>
   1.4 [sge_scene_section_header](#sge_scene_section_header) <br>
   1.5 [sge_scene_section](#sge_scene_section) <br>
   1.6 [sge_scene](#sge_scene) <br>
   1.7 [sge_scene_settings](#sge_scene_settings) <br>
2. [Defines](#defines) <br>
   2.1 [SGE_SCENE_MAGIC_NUMBER](#sge_scene_magic_number) <br>
   2.2 [SGE_SCENE_HEADER_FIXED_SIZE](#sge_scene_header_fixed_size) <br>
   2.3 [SGE_SCENE_SECTION_HEADER_FIXED_SIZE](#sge_scene_section_header_fixed_size) <br>
   2.4 [SGE_TRANSFORMATION_POSITION_SIZE](#sge_transformation_position_size) <br>
   2.5 [SGE_TRANSFORMATION_SCALE_SIZE](#sge_transformation_scale_size) <br>
   2.6 [SGE_TRANSFORMATION_ROTATION_SIZE](#sge_transformation_rotation_size) <br>
3. [Enumerations](#enumerations) <br>
   3.1 [SGE_SCENE_SGEREND_INCLUDE_TYPE](#sge_scene_sgerend_include_type) <br>
   3.2 [SGE_SCENE_SECTION_TYPE](#sge_scene_section_type) <br>
   3.3 [SGE_SCENE_TRANSFORMATION_FLAGS](#sge_scene_transformation_flags) <br>
4. [Unions](#unions) <br>
   4.1 [sge_scene_section_data](#sge_scene_section_data) <br>
5. [API Reference](#api-reference) <br>
   5.1 [sge_scene_create](#sge_scene-sge_scene_create) <br>
   5.2 [sge_scene_create_sgerend_section](#sge_scene_section-sge_scene_create_sgerend_section) <br>
   5.3 [sge_scene_add_section](#sge_result-sge_scene_add_section) <br>
   5.4 [sge_scene_save](#sge_result-sge_scene_save) <br>
   5.5 [sge_scene_load](#sge_scene-sge_scene_load) <br>
   5.6 [sge_scene_parse_sgerend_section](#sge_result-sge_scene_parse_sgerend_section)
6. [Examples](#examples) <br>
   6.1 [Create a Scene](#create-a-scene) <br>
   6.2 [Create Section](#create-section) <br>
   6.3 [Add Section to Scene](#add-section-to-scene) <br>
   6.4 [Save a Scene](#save-a-scene) <br>
   6.5 [Load a Scene](#load-a-scene) <br>

## Structures

### sge_scene_extension

```c
typedef struct sge_scene_extension {
    uint16_t        type;
    uint32_t        data_size;
    uint8_t*        data;
} sge_scene_extension;
```

---

### sge_scene_header
```c
typedef struct sge_scene_header {
    uint16_t                major_version;
    uint16_t                minor_version;
    uint16_t                patch_version;
    uint32_t                section_count;
    uint32_t                scene_name_size;
    char                    *scene_name;
    uint32_t                author_name_size;
    char                    *author_name;
    uint64_t                creation_date_timestamp;
    uint64_t                last_modified_date_timestamp;
    uint32_t                description_size;
    char                    *description;
    uint16_t                header_extension_count;
    sge_scene_extension     *header_extensions;
    size_t                  header_extension_size;
    uint32_t                crc32_checksum;
} sge_scene_header;
```

---

### sge_scene_sgerend_section
```c
typedef struct sge_scene_sgerend_section {
    uint8_t                 include_type; //SGE_SCENE_SGEREND_INCLUDE_TYPE
    uint16_t                additional_section_count;
    union {
        uint16_t            sgerend_source_size_non_embedded; //if filepath
        uint32_t            sgerend_source_size_embedded; //if embedded
    };
    void                    *sgerend_source_data;
    uint32_t                section_data_size;      //only used if additional section count > 0
    sge_rend_section        *additional_sge_rend_sections;
    uint16_t                transformation_flags; //SGE_SCENE_TRANSFORMATION_FLAGS
    void                    *transformation_data;
} sge_scene_sgerend_section;
```

---

### sge_scene_section_header
```c
typedef struct sge_scene_section_header {
    uint16_t                sge_scene_section_type; //SGE_SCENE_SECTION_TYPE, but force to 2 byte
    uint64_t                section_offset;
    uint64_t                data_size;
    uint32_t                section_name_size;
    char                    *section_name;
    uint64_t                creation_date_timestamp;
    uint64_t                last_modified_date_timestamp;
    uint16_t                section_extension_count;
    size_t                  section_extension_size;
    sge_scene_extension     *extensions;
    uint32_t                crc32_checksum;
    size_t                  header_size;
} sge_scene_section_header;
```

---

### sge_scene_section
```c
typedef struct sge_scene_section {
    sge_scene_section_header        *section_header;
    void                            *data;
    sge_scene_section_data          *parsed_data;
} sge_scene_section;
```

---

### sge_scene
```c
typedef struct sge_scene {
    sge_scene_header        header;
    sge_scene_section       *sections;
} sge_scene;
```

---

### sge_scene_settings
```c
typedef struct sge_scene_settings {
    char    *filename;
} sge_scene_settings;
```

## Defines

### SGE_SCENE_MAGIC_NUMBER
```c
#define SGE_SCENE_MAGIC_NUMBER "SGESCNE" //has 0 terminator automatically so 8 bytes
```

---

### SGE_SCENE_HEADER_FIXED_SIZE
```c
#define SGE_SCENE_HEADER_FIXED_SIZE 52
```

---

### SGE_SCENE_SECTION_HEADER_FIXED_SIZE
```c
#define SGE_SCENE_SECTION_HEADER_FIXED_SIZE 44
```

---

### SGE_TRANSFORMATION_POSITION_SIZE
```c
#define SGE_TRANSFORMATION_POSITION_SIZE  12 //3x float32 (vec3)
```

---

### SGE_TRANSFORMATION_SCALE_SIZE
```c
#define SGE_TRANSFORMATION_SCALE_SIZE     12 //3x float32 (vec3)
```

---

### SGE_TRANSFORMATION_ROTATION_SIZE
```c
#define SGE_TRANSFORMATION_ROTATION_SIZE  12 //3x float32 (vec3)
```

## Enumerations

### SGE_SCENE_SGEREND_INCLUDE_TYPE
```c
typedef enum SGE_SCENE_SGEREND_INCLUDE_TYPE {
    SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL = 0,
    SGE_SCENE_SGEREND_INCLUDE_TYPE_EMBEDDED = 1,
} SGE_SCENE_SGEREND_INCLUDE_TYPE;
```

### SGE_SCENE_SECTION_TYPE
```c
typedef enum SGE_SCENE_SECTION_TYPE {
    SGE_SCENE_SECTION_TYPE_SGEREND = 1,
} SGE_SCENE_SECTION_TYPE;
```

### SGE_SCENE_TRANSFORMATION_FLAGS
```c
typedef enum SGE_SCENE_TRANSFORMATION_FLAGS {
    SGE_SCENE_TRANSFORMATION_FLAG_POSITION  = 0x01,
    SGE_SCENE_TRANSFORMATION_FLAG_SCALE     = 0x02,
    SGE_SCENE_TRANSFORMATION_FLAG_ROTATION  = 0x04,
    SGE_SCENE_TRANSFORMATION_FLAG_MAX       = 0xFF,
} SGE_SCENE_TRANSFORMATION_FLAGS;
```

## Unions

### sge_scene_section_data
```c
typedef union sge_scene_section_data {
    sge_scene_sgerend_section       *sgerend;
} sge_scene_section_data;
```

## API Reference

### sge_scene *sge_scene_create(...)

```c
sge_scene *sge_scene_create(char *scene_name, char *author_name, char *description);
```

#### Parameters:
- `scene_name`: Pointer to char containing scene name
- `author_name`: Pointer to char containing author name
- `description`: Pointer to char containing description

#### Returns:
- `Pointer`: to `sge_scene` struct, that got created
- `NULL`: if creation failed

---

### sge_scene_section *sge_scene_create_sgerend_section(...)

```c
sge_scene_section *sge_scene_create_sgerend_section(
         SGE_SCENE_SGEREND_INCLUDE_TYPE include_type, 
         char *section_name, 
         void *source_data, 
         size_t source_data_size, 
         uint16_t transformation_flags, 
         void *transformation_data);
```

#### Parameters:
- `include_type`: Whether to embed the data or reference it externally
- `section_name`: Name of the section
- `source_data`: Pointer to the source (embedded data or file path)
- `source_data_size`: Embedded size or external length
- `transformation_flags`: `SGE_SCENE_TRANSFORMATION_FLAGS` flags defining what transformations are used->{start with position, then scale, then rotation}
- `transformation_data`: Transformation struct pointer

#### Returns:
- `Pointer`: to the created `sge_scene_section`
- `NULL`: if it failed

---

### SGE_RESULT sge_scene_add_section(...)

```c
SGE_RESULT sge_scene_add_section(sge_scene *scene, sge_scene_section *section);
```

#### Parameters:
- `scene`: Scene to add section to
- `section`: Section to be added

#### Returns:
- `SGE_INVALID_API_CALL`: if not all required arguments are passed
- `SGE_ERROR_FAILED_ALLOCATION`: if the allocation failed
- `SGE_SUCCESS`: on success

---

### SGE_RESULT sge_scene_save(...)

```c
SGE_RESULT sge_scene_save(char *filename, sge_scene *scene);
```

#### Parameters:
- `filename`: Output file name
- `scene`: Scene to save

#### Returns:
- `SGE_INVALID_API_CALL`: if not all required arguments are passed
- `SGE_ERROR_FAILED_ALLOCATION`: if the allocation failed
- `SGE_SUCCESS`: on success

---

### sge_scene *sge_scene_load(...)

```c
sge_scene *sge_scene_load(char *filename);
```

#### Parameters:
- `filename`: Input file name to load

#### Returns:
- `Pointer`: to loaded scene object
- `NULL`: if it failed to load

---

### SGE_RESULT sge_scene_parse_sgerend_section(...)

```c
SGE_RESULT sge_scene_parse_sgerend_section(sge_scene_section_data **parsed_output, void *data, size_t data_size);
```

#### Parameters:
- `parsed_output`: Output parsed structure
- `data`: Raw section data
- `data_size`: Size of the data

#### Returns:
- `SGE_ERROR_FAILED_ALLOCATION`: if the allocation failed
- `SGE_SUCCESS`: on success
- `SGE_ERROR`: on error

---

## Examples

### Create a Scene

```c
char* author_name = "Stein";
char* scene_name = "test_scene";

sge_scene *scene = sge_scene_create(scene_name, author_name, NULL);
```

---

### Create Section

```c
vec3 position_transformation = {2, -1, -1};
sge_scene_section *sgerend_section = sge_scene_create_sgerend_section(
       SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
       "Test Cube Section",
       "cube.sgerend",
       strlen("cube.sgerend"),
       SGE_SCENE_TRANSFORMATION_FLAG_POSITION,
       &position_transformation);

typedef struct scale_position_trans {
       vec3 position_trans;
       vec3 scale_trans;
}scale_position_trans;

scale_position_trans sca_pos_trans = {
       .position_trans =  {2, -1, -1},
       .scale_trans = {2, 2, 1}
};

sge_scene_section *pyramid_section = sge_scene_create_sgerend_section(
       SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
       "TEST PYRAMID",
       "test.sgerend",
       strlen("test.sgerend"),
       SGE_SCENE_TRANSFORMATION_FLAG_POSITION | SGE_SCENE_TRANSFORMATION_FLAG_SCALE,
       &sca_pos_trans);
```

---

### Add Section to Scene

```c
sge_scene_add_section(scene, sgerend_section);
sge_scene_add_section(scene, pyramid_section);
```

---

### Save a Scene
```c
sge_scene_save("test_scene", scene);
```

---

### Load a Scene

```c
sge_scene *scene_loaded = sge_scene_load("test_scene.sgescne");
if (!scene_loaded) {
        terminate_program();
}
```