# SGE Texture File format

## Overview

The `.sgetext` file format is used in the SGE (Stein Game Engine) to apply and manage Textures on renderables 

## Table of Content

1. [Header](#header) <br>
2. [Section Header](#section-header) <br>
   2.1 [Section Types](#section-types) <br>
3. [Extensions](#extensions) <br>
4. [Examples](#examples) <br>

## Header

The header defines general information about the `.sgetext` file. Its initial size is 40 Bytes, excluding variable-length fields such as names and extensions. Total size is calculated as:

**48 (Base Size) + T + A + D + E**, where:
- **T** = Texture Name Size
- **A** = Author Name Size
- **D** = Description Size
- **E** = Extension Data Size

| Offset               | Size    | Name                         | Description                         |
|----------------------|---------|------------------------------|-------------------------------------|
| 0x00                 | 8 Bytes | Magic Number                 | `SGETEXT\0`                         |
| 0x08                 | 2 Bytes | Major Version                | 16-bit integer                      |
| 0x0A                 | 2 Bytes | Minor Version                | 16-bit integer                      |
| 0x0C                 | 2 Bytes | Patch Version                | 16 bit                              |
| 0x0E                 | 4 Bytes | Section Count                | Number of sections in the file      |
| 0x12                 | 4 Bytes | Texture Name Size            | Determines `T Bytes`                |
| 0x16                 | T Bytes | Texture Name                 | Non Null Terminated                 |
| 0x16 + T             | 4 Bytes | Author Name Size             | Determines `A Bytes`                |
| 0x1A + T             | A Bytes | Author Name                  | Non Null Terminated                 |
| 0x1A + T + A         | 8 Bytes | Creation Date Timestamp      | Unix Timestamp                      |
| 0x22 + T + A         | 8 Bytes | Last Modified Date Timestamp | Unix Timestamp                      |
| 0x2A + T + A         | 4 Bytes | Description Size             | Determines `D Bytes`                |
| 0x2E + T + A         | D Bytes | Description                  | Non Null Terminated                 |
| 0x2E + T + A + D     | 2 Bytes | Header Extension Count       | Number of extensions included       |
| 0x30 + T + A + D     | E Bytes | Header Extension Data        | Optional data for format extensions |
| 0x30 + T + A + D + E | 4 Bytes | Header Checksum              | CRC32 Checksum `"0xEDB88320"`       |

---

## Section Header


| Offset       | Size    | Name                         | Description                           |
|--------------|---------|------------------------------|---------------------------------------|
| 0x00         | 1 Bytes | Section Type                 | Specifies the type of texture section |
| 0x01         | 8 Bytes | Section Offset               | File offset where the section starts  |
| 0x09         | 8 Bytes | Data Size                    | Size of section data excluding header |
| 0x11         | 4 Bytes | Section Name Size            | Populates `S Bytes`                   |
| 0x15         | S Bytes | Section Name                 | Unique name identifying the texture   |
| 0x15 + S     | 8 Bytes | Creation Date Timestamp      | Unix Timestamp                        |
| 0x1D + S     | 8 Bytes | Last Modified Date Timestamp | Unix Timestamp                        |
| 0x25 + S     | 2 Bytes | Section Extension Count      | Number of extensions in the section   |
| 0x27 + S     | E Bytes | Section Extension Data       | Optional extension data               |
| 0x27 + S + E | 4 Bytes | Section Checksum             | CRC32 Checksum                        |

---

### Section Types

| Type   | Value | Name                    | Description   |
|:-------|:------|:------------------------|:--------------|
| Color  | 1     | SGE_TEXTURE_TYPE_COLOR  | Color Section |
| Normal | 2     | SGE_TEXTURE_TYPE_NORMAL |               |
| Height | 3     | SGE_TEXTURE_TYPE_HEIGHT |               |


## Section Bodies 

### Color Body

Body is based on Include Type

**include_type = 0 (SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL)**

| Offset | Size    | Name           | Description         |
|:-------|:--------|:---------------|:--------------------|
| 0x00   | 1 Byte  | Include Type   | 0 (External)        |
| 0x01   | 4 Byte  | FilePathLength | Fills `FS Bytes`    |
| 0x05   | FS Byte | Filepath       | Non null terminated |

**include_type = 1 (SGE_SCENE_SGEREND_INCLUDE_TYPE_EMBEDDED)**

| Offset | Size    | Name                            | Description                                        |
|:-------|:--------|:--------------------------------|:---------------------------------------------------|
| 0x00   | 1 Byte  | Include Type                    | 0 (External)                                       |
| 0x01   | 1 Byte  | `SGE_TEXTURE_FORMAT_COLOR_TYPE` | Texture format                                     |
| 0x03   | 1 Byte  | Bit Depth                       | 1, 2, 4, 8, 16, must be compatible with color type |
| 0x05   | 4 Byte  | Width                           | Height in Pixels                                   |
| 0x0D   | 4 Byte  | Height                          | Height in Pixels                                   |
| 0x11   | 8 Byte  | Data Size                       | Size of Binary Data                                |
| 0x19   | DS Byte | Parsed Data                     | Raw Color Data                                     |


### Normal Body
// LATER
### Height Body
//LATER


## Enumerations

### SGE_TEXTURE_FORMAT_COLOR_TYPE

```c
typedef enum SGE_TEXTURE_FORMAT_COLOR_TYPE{
   SGE_TEXTURE_FORMAT_COLOR_TYPE_GRAYSCALE = 0,
   SGE_TEXTURE_FORMAT_COLOR_TYPE_TRUECOLOR = 2,
   SGE_TEXTURE_FORMAT_COLOR_TYPE_INDEXED = 3,
   SGE_TEXTURE_FORMAT_COLOR_TYPE_GRAYSCALE_WITH_ALPHA = 4,
   SGE_TEXTURE_FORMAT_COLOR_TYPE_TRUECOLOR_WITH_ALPHA = 6,
} SGE_TEXTURE_FORMAT_COLOR_TYPE;
```

## Extensions

## Examples
