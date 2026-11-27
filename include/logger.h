#pragma once

#include <stdarg.h>
#include <stdio.h>

// makes GCC/Clang check your format string against the arguments, like printf
#if defined(__GNUC__) || defined(__clang__)
#define LOG_FORMAT_CHECK(fmt_index, args_index) __attribute__((format(printf, fmt_index, args_index)))
#else
#define LOG_FORMAT_CHECK(fmt_index, args_index)
#endif

inline void log_print(const char* prefix, const char* format, va_list args) {
    printf("%s", prefix);
    vprintf(format, args);
    printf("\n");
}

inline void log_fatal(const char* format, ...) LOG_FORMAT_CHECK(1, 2);
inline void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_print("MARCHPHINE (FATAL!): ", format, args);
    va_end(args);
}

inline void log_warning(const char* format, ...) LOG_FORMAT_CHECK(1, 2);
inline void log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_print("MARCHPHINE (WARNING): ", format, args);
    va_end(args);
}

inline void log_debug(const char* format, ...) LOG_FORMAT_CHECK(1, 2);
inline void log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_print("MARCHPHINE (DEBUG): ", format, args);
    va_end(args);
}