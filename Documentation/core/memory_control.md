# SGE Memory Control System

## Overview
The SGE Memory Control System provides memory management, including allocation, deallocation, and tracking using `memory_tag`. It tracks each allocation and associates the amount with its tag.

## Memory Tags
Memory tags categorize memory allocations, used in tracking.

| Tag                   | Value | Description                        |
|-----------------------|-------|------------------------------------|
| `MEMORY_TAG_UNKNOWN`  | 0     | Unclassified memory allocation     |
| `MEMORY_TAG_LOGGER`   | 1     | Logging system allocations         |
| `MEMORY_TAG_INPUT`    | 2     | Input system allocations           |
| `MEMORY_TAG_VULKAN`   | 3     | Vulkan-related memory              |
| `MEMORY_TAG_RENDERER` | 4     | Renderer-specific allocations      |
| `MEMORY_TAG_REGION`   | 5     | Render Region specific allocations |
| `MEMORY_TAG_WINDOW`   | 6     | Window system allocations          |
| `MEMORY_TAG_SHADER`   | 7     | Shader system allocations          |

## Adding Custom Tags
To add custom tags for your application, modify the header file by adding a new enum value. Additionally, update `memory_tag_to_string` to ensure proper string logging in `print_memory_usage_str`.

### Example:
```c
memory_control.h

typedef enum memory_tag {
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_LOGGER,
        MEMORY_TAG_INPUT,
        MEMORY_TAG_VULKAN,
        MEMORY_TAG_RENDERER,
        MEMORY_TAG_REGION,
        MEMORY_TAG_WINDOW,
        MEMORY_TAG_SHADER,
        MEMORY_TAG_CUSTOM, // New custom tag
} memory_tag;

static inline const char *memory_tag_to_string(const memory_tag tag) {
        switch (tag) {
                case MEMORY_TAG_UNKNOWN: return "UNKNOWN";
                case MEMORY_TAG_LOGGER: return "LOGGER";
                case MEMORY_TAG_INPUT: return "INPUT";
                case MEMORY_TAG_VULKAN: return "VULKAN";
                case MEMORY_TAG_RENDERER: return "RENDERER";
                case MEMORY_TAG_REGION: return "REGION";
                case MEMORY_TAG_WINDOW: return "WINDOW";
                case MEMORY_TAG_SHADER: return "SHADER";
                case MEMORY_TAG_CUSTOM: return "CUSTOM"; // New tag case
                default: return "INVALID_TAG";
        }
}
```

## API Reference

### `void *allocate_memory(size_t size, memory_tag tag);`
Allocates and zeroes a memory block of the specified `size`, associating it with a `memory_tag`.

#### Parameters:
- `size`: Number of bytes to allocate.
- `tag`: The `memory_tag` associated with the allocation.

#### Returns:
- A valid pointer on success.
- `NULL` on failure.

---

### `void free_memory(void *ptr, memory_tag tag);`
Frees memory associated with `ptr`.

#### Parameters:
- `ptr`: Pointer to the memory to be freed.
- `tag`: The `memory_tag` associated with the allocation.

Logs an error if attempting to free an untracked or `NULL` pointer.

If a memory tag’s usage underflows (becomes negative due to incorrect tracking), an error is logged.

---

### `void *reallocate_memory(void *old_ptr, size_t new_size, memory_tag tag);`
Resizes an existing allocation while maintaining its tag.

#### Parameters:
- `old_ptr`: Pointer to the existing memory block (can be `NULL`).
- `new_size`: New size in bytes.
- `tag`: The `memory_tag` associated with the allocation.

#### Returns:
- A valid pointer on success.
- `NULL` on failure.

If `old_ptr` is `NULL`, behaves like `allocate_memory`. If `new_size` is `0`, behaves like `free_memory`.

---

### `void *zero_memory(void *ptr, size_t size, int offset);`
Sets `size` bytes to zero starting from `ptr + offset`.

#### Parameters:
- `ptr`: Pointer to the memory block.
- `size`: Number of bytes to set to zero.
- `offset`: Offset from `ptr` where zeroing starts.

Logs an error if `ptr` is `NULL`.

---

### `void *copy_memory(void *dest_ptr, void *src_ptr, size_t size, size_t offset_dest, size_t offset_src);`
Copies `size` bytes from `src_ptr + offset_src` to `dest_ptr + offset_dest`.

#### Parameters:
- `dest_ptr`: Destination memory block.
- `src_ptr`: Source memory block.
- `size`: Number of bytes to copy.
- `offset_dest`: Offset in destination.
- `offset_src`: Offset in source.

Logs an error if either pointer is `NULL`.

---

### `void *set_memory(void *ptr, int value, size_t size, int offset);`
Sets `size` bytes to `value` starting from `ptr + offset`.

#### Parameters:
- `ptr`: Pointer to the memory block.
- `value`: Value to set.
- `size`: Number of bytes to modify.
- `offset`: Offset from `ptr` where modification starts.

Logs an error if `ptr` is `NULL`.

---

### `void print_memory_usage_str();`
Prints a formatted, sorted memory usage report, including total memory usage tracker usage and each memory tag amount.

### Example

```
--------------------------------------------------
|              Current Memory Usage              |
--------------------------------------------------
| Total Usage   | 8631 Bytes                     |
--------------------------------------------------
| Tracker Usage | 640 Bytes                      |
--------------------------------------------------
| LOGGER   | 4191 Bytes                          |
--------------------------------------------------
| INPUT    | 2176 Bytes                          |
--------------------------------------------------
| VULKAN   | 1424 Bytes                          |
--------------------------------------------------
| RENDERER | 176 Bytes                           |
--------------------------------------------------
| WINDOW   | 24 Bytes                            |
--------------------------------------------------
```

---

## Memory Tracking

The system maintains a linked list of allocations and a second to track usage per tag 

```c
//For each allocation to track it and later free it
typedef struct allocation_info {
        void *ptr;
        size_t size;
        memory_tag tag;
        struct allocation_info *next;
} allocation_info;

//To track the usage of each memory tag
typedef struct memory_tag_usage {
        memory_tag tag;
        size_t usage;
        struct memory_tag_usage *next;
} memory_tag_usage;
```