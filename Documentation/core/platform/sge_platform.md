# SGE PLATFORM Module

## Overview

## Table of Content
1. [API Reference](#api-reference) <br>
   1.1 [os_sleep](#void-os_sleep)
2. [Examples](#examples) <br>
   2.1 [sleep](#sleep) 

## API Reference

### void os_sleep(...)

**Important Note**
- Windows Sleeps for around 15,6 ms

```c
void os_sleep(uint32_t sleep_time);
```

#### Parameters:
- `sleep_time`: Number of Milliseconds to Sleep

## Examples

### Sleep

```c
printf("Test\n");
os_sleep(2000);
printf("Test 2 secs later\n");
```