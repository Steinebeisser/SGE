# SGE PLATFORM Module

## Overview

## Table of Content
1. [API Reference](#api-reference)
2. [Examples](#examples)

## API Reference

### char *get_cwd(...)

```c
char *get_cwd();
```

#### Returns:
- `Pointer` to a null terminated string containing current working directory of application

---

### void os_sleep(...)

**Important Note**
- Windows Sleeps for around 15,6 ms

```c
void os_sleep(uint32_t sleep_time);
```

#### Parameters:
- `sleep_time`: Number of Milliseconds to Sleep

