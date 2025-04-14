# SGE CRC32 Module

## Overview
The SGE crc32 Module is used to create a checksum of data for integrity the polynom used is `0xEDB88320`

## Table of Content

1. [API Reference](#api-reference) <br>
2. [Examples](#examples) <br>
   2.1 [Creating CRC32 Checksum](#creating-crc32-checksum) <br>

## API Reference

### uint32_t calculate_crc32(...)

```c
uint32_t calculate_crc32(void *data, size_t length);
```

#### Parameters:
- `data`: Pointer to the data the checksum should be created for
- `length`: The length of the data in bytes

#### Returns:
- `uint32_t`: 4 Byte crc32 Checksum

---

## Examples

### Creating CRC32 Checksum

```c
char *data = "testerino";
size_t length = strlen(data);

uint32_t crc32_checksum = calculate_crc32(data, length);

printf("Checksum of string \"%s\" is \"%d\"\n", data, crc32_checksum);
```

**Output**
- `Checksum of string "testerino" is "1877997484"`

---