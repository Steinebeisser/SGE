# SGE Utils Module

## Overview
The SGE Utils Module provides utility functions for handling program termination, shutdown, and sorting linked lists. These functions are designed to assist with managing program flow and data structures within the SGE engine.

## Table of Content

1. [API Reference](#api-reference)
   1.1 [terminate_program](#void-terminate_program)  
   1.2 [shutdown_program](#void-shutdown_program)  
   1.3 [sort_linked_list](#void-sort_linked_list)
2. [Examples](#examples)
   2.1 [Sort a Linked List](#sorting-linked-list)

## API Reference

### `void terminate_program()`
calls stop_logger and exits with code 1
```c
void terminate_program();
```

---

### `void shutdown_program()`
calls stop_logger and exits with code 0
```c
void shutdown_program();
```
---

### `void* sort_linked_list(...)`
```c
void* sort_linked_list(void *head_ptr, size_t offset_sort_attribute, size_t offset_next, size_t sort_attribute_size, size_t next_size, bool reversed, void *(*get_next)(const void *));
```
#### Parameters:
- `head_ptr`: Pointer to the head of the linked list.
- `offset_sort_attribute`: The offset of the attribute to sort by within each linked list element.
- `offset_next`: The offset of the pointer to the next element in the list.
- `sort_attribute_size`: The size of the attribute to be sorted by.
- `next_size`: The size of the next pointer.
- `reversed`: A boolean indicating whether the list should be sorted in descending order (`true`) or ascending order (`false`).
- `get_next`: A function pointer that returns the next element in the linked list.

#### Returns:
- `Pointer`: to head of sorted linked list

---

## Examples

### Sorting linked list
 Struct
```c
typedef struct memory_tag_usage {
        memory_tag tag;
        size_t usage;
        struct memory_tag_usage *next;
} memory_tag_usage;
```
Next Node Function
```c
void *get_next_allocation_memory_tag_usage(const void *node) {
        return ((memory_tag_usage *)node)->next;
}
```
Sorting call, sorting based on `usage`
```c
sort_linked_list(memory_tag_usage_tracker, 
                  offsetof(memory_tag_usage, usage), 
                  offsetof(memory_tag_usage, next), 
                  sizeof(size_t), 
                  sizeof(memory_tag_usage), 
                  true, 
                  get_next_allocation_memory_tag_usage);
```
