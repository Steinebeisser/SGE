# SGE Logging System

## Overview
The SGE Logging System provides a structured way to log messages at different severity levels. It supports immediate writing to logs and can be started and stopped as needed.

## Log Levels
The logging system supports the following log levels:

| Level               | Value | Description                            |
|---------------------|-------|----------------------------------------|
| `LOG_LEVEL_FATAL`   | 0     | Critical errors that cause termination |
| `LOG_LEVEL_ERROR`   | 1     | Errors that cause runtime failure      |
| `LOG_LEVEL_WARNING` | 2     | Warnings about potential issues        |
| `LOG_LEVEL_INFO`    | 3     | General information messages           |
| `LOG_LEVEL_DEBUG`   | 4     | Debugging messages                     |
| `LOG_LEVEL_TRACE`   | 5     | Detailed trace for debugging           |

## API Reference

### `SGE_RESULT start_logger(sge_log_settings settings);`
Starts the logging system with the provided settings. It creates a folder structure in the execution directory:

```
logs/{current_year}/{current_month}/{current_day}
```

Within this folder, it creates a log file:

```
{current_day}-{current_month}-{current_year}_{current_hour}-{current_min}-{current_sec}.log
```

#### Example Path:
```
C:\Users\Geisthardt\CLionProjects\SGE\cmake-build-debug\logs\2025\03\31\31-03-2025_15-36-36.log
```

#### Parameters:
- `settings`: `sge_log_settings` structure containing configuration options.

#### Returns:
- `SGE_RESULT`: Status code indicating success or failure.

---

### `SGE_RESULT stop_logger();`
Stops the logging system and performs necessary cleanup. Writes everything that's still in the buffer to the log file.

#### Returns:
- `SGE_RESULT`: Status code indicating success or failure.

---

### `void log_event(log_level level, const char *message, ...);`
Logs an event at the specified log level.

#### Parameters:
- `level`: Log level (one of `LOG_LEVEL_*` values).
- `message`: Format string for the log message.
- `...`: Additional arguments for formatted output.

If `LOG_LEVEL_FATAL` is used, the application terminates and `stop_logger` is called to clean up and write to the file.

---

## Configuration

### `sge_log_settings` Structure

| Field                   | Type       | Description                                                                                                                                                            |
|-------------------------|------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `write_instantly`       | `SGE_BOOL` | If `SGE_TRUE`, logs are written immediately. Otherwise, they are buffered and batch written.                                                                           |
| `include_internal_logs` | `SGE_BOOL` | If `SGE_TRUE`, internal logs are included like `[   INFO]  [31.03.2025 15:36:36]  [INTERNAL] Log started`.                                                             |
| `is_release`            | `SGE_BOOL` | If `SGE_TRUE`, only log `LOG_LEVEL_FATAL`, `LOG_LEVEL_ERROR` and `LOG_LEVEL_WARNING`, therefore supressing `LOG_LEVEL_INFO`, `LOG_LEVEL_DEBUG`, and `LOG_LEVEL_TRACE`. |                                                                                            

## Usage Example
```c
sge_log_settings settings = { .write_instantly = true };
if (start_logger(settings) != SGE_SUCCESS) {
    printf("Failed to start logger\n");
    return -1;
}

log_event(LOG_LEVEL_INFO, "Logger initialized.");
log_event(LOG_LEVEL_ERROR, "Parameter usage: %d", 69);

stop_logger();
```