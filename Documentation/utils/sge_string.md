## SGE String Utils

## Overview

SGE String Utils provides utility functions for handling and manipulating strings within the SGE engine.

## Table of Contents

1. [API Reference](#api-reference)
   1.1 [int get_longest_element_in_array](#int-get_longest_element_in_array)
   1.2 [SGE_RESULT get_last_string_index](#sge_result-get_last_string_index)
   1.3 [SGE_RESULT get_file_ending](#sge_result-get_file_ending)
   1.4 [SGE_BOOL string_includes](#sge_bool-string_includes)
2. [Examples](#examples)
   2.1 [Finding longest element in array](#finding-longest-element-in-array)
   2.2 [Last occurrence of character](#last-occurrence-of-character)
   2.3 [Extracting file extension](#extracting-file-extension)
   2.4 [Checking if a string includes another string](#checking-if-a-string-includes-another-string)


## API Reference

### int get_longest_element_in_array(...)

```c
int get_longest_element_in_array(char *array[]);
```

#### Parameters:
- `array`: A null-terminated array of strings.

#### Returns:
- The length of the longest string in the array.

---

### SGE_RESULT get_last_string_index(...)

```c 
SGE_RESULT get_last_string_index(char *string, char element, size_t *index_var); 
```

#### Parameters:
- `string`: The input string.
- `element`: The character to find the last occurrence of.
- `index_var`: Pointer to store the last index of `element` in `string`.

#### Returns:
- `SGE_SUCCESS` on success

---

### SGE_RESULT get_file_ending(...)

```c
SGE_RESULT get_file_ending(char *filename, char *file_ending);
```

#### Parameters:
- `filename`: The input filename.
- `file_ending`: A buffer to store the file extension.

#### Returns:
- `SGE_SUCCESS` on success.

---

### SGE_BOOL string_includes(...)

```c
SGE_BOOL string_includes(char *string, char *check_string);
```

#### Parameters:
- `string`: The main string.
- `check_string`: The substring to check for.

#### Returns:
- `SGE_TRUE` if `check_string` is found within `string`, otherwise `SGE_FALSE`.

---

## Examples

### Finding longest element in array

```c
char *words[] = {"apple", "banana", "cherry", "watermelon", NULL};
int longest = get_longest_element_in_array(words);
printf("Longest string length: %d\n", longest);
```

**Output:**
```
Longest string length: 10
```

---

### Last occurrence of character

```c
char text[] = "hello.world.txt";
size_t index;
get_last_string_index(text, '.', &index);
printf("Last '.' found at index: %zu\n", index);
```

**Output:**
```
Last '.' found at index: 11
```

---

### Extracting file extension

```c
char filename[] = "document.pdf";
char extension[10];
get_file_ending(filename, extension);
printf("File extension: %s\n", extension);
```

**Output:**
```
File extension: pdf
```

---

### Checking if a string includes another string

```c
char haystack[] = "The quick brown fox";
char needle[] = "quick";
if (string_includes(haystack, needle)) {
    printf("Substring found!\n");
} else {
    printf("Substring not found.\n");
}
```

**Output:**
```
Substring found!
```

