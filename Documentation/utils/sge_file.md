## SGE File Utils

## Overview

SGE File Utils provides essential functions for file and directory operations within the SGE engine.

## Table of Contents

1. [API Reference](#api-reference)
   1.1 [void make_filename_save](#void-make_filename_save)
   1.2 [SGE_BOOL create_directory_if_not_exists](#sge_bool-create_directory_if_not_exists)
   1.3 [char *get_current_working_directory](#char-get_current_working_directory)
   1.4 [SGE_BOOL sge_file_exists](#sge_bool-sge_file_exists)
   1.5 [uint32_t *read_file_as_binary](#uint32_t-read_file_as_binary)
2. [Examples](#examples)
   2.1 [Making a filename safe](#making-a-filename-safe)
   2.2 [Creating a directory if it does not exist](#creating-a-directory-if-it-does-not-exist)
   2.3 [Getting the current working directory](#getting-the-current-working-directory)
   2.4 [Checking if a file exists](#checking-if-a-file-exists)
   2.5 [Reading a file as binary](#reading-a-file-as-binary)

---

## API Reference

### void make_filename_save(...)

```c
void make_filename_save(char *filename);
```

#### Parameters:
- `filename`: The filename to be made save

#### Description:
- Replaces unsafe characters (`:` and `.`) with `-` and spaces with `_` to make the filename safe.

---

### SGE_BOOL create_directory_if_not_exists(...)

```c
SGE_BOOL create_directory_if_not_exists(char *dir_path);
```

#### Parameters:
- `dir_path`: The directory path to check and create if it does not exist.

#### Returns:
- `SGE_TRUE` if the directory exists or was successfully created.
- `SGE_FALSE` if the directory creation failed.

---

### char *get_current_working_directory(...)

```c
char *get_current_working_directory();
```

#### Returns:
- A dynamically allocated string containing the current working directory.

---

### SGE_BOOL sge_file_exists(...)

```c
SGE_BOOL sge_file_exists(char *filepath);
```

#### Parameters:
- `filepath`: The path of the file to check.

#### Returns:
- `SGE_TRUE` if the file exists.
- `SGE_FALSE` otherwise.

---

### uint32_t *read_file_as_binary(...)

```c
uint32_t *read_file_as_binary(const char *filepath, size_t *code_size);
```

#### Parameters:
- `filepath`: The path to the file.
- `code_size`: A pointer to store the size of the file.

#### Returns:
- A pointer to the allocated memory containing the file data.

---

## Examples

### Making a filename safe

```c
char filename[] = "test:file.txt";
make_filename_save(filename);
printf("Safe filename: %s\n", filename);
```

**Output:**
```
Safe filename: test-file-txt
```

### Creating a directory if it does not exist

```c
char dir[] = "./new_folder";
if (create_directory_if_not_exists(dir)) {
    printf("Directory created or already exists\n");
} else {
    printf("Failed to create directory\n");
}
```

**Output:**
```
Directory created or already exists
```

### Getting the current working directory

```c
char *cwd = get_current_working_directory();
printf("Current working directory: %s\n", cwd);
free(cwd);
```

**Output:**
```
Current working directory: /home/user/project
```

### Checking if a file exists

```c
char *filepath = "./test.txt";
if (sge_file_exists(filepath)) {
    printf("File exists\n");
} else {
    printf("File does not exist\n");
}
```

**Output:**
```
File exists
```

### Reading a file as binary

```c
size_t file_size;
uint32_t *file_data = read_file_as_binary("./binary.dat", &file_size);
printf("Read %zu bytes from file\n", file_size);
free(file_data);
```

**Output:**
```
Read 1024 bytes from file
```

