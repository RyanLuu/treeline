#include "core/logging.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <unordered_map>
#include <utility>

std::unordered_map<FILE *, LogPriority> g_logHandlers;

#ifdef DEBUG_BUILD
constexpr LogPriority DEFAULT_PRIORITY = DEBUG;
#else
constexpr LogPriority DEFAULT_PRIORITY = INFO;
#endif

void logInit() {
    g_logHandlers.emplace(std::make_pair(stdout, DEFAULT_PRIORITY));
}

const char *priorityString(LogPriority priority) {
    switch (priority) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO ";
        case WARN:
            return "WARN ";
        case ERROR:
            return "ERROR";
    }
}

const char *basename(const char *filename) {
    const char *ret = filename;
    for (size_t i = 0; filename[i] != 0; i++) {
        if (filename[i] == '/') {
            ret = &filename[i + 1];
        }
    }
    return ret;
}

void log(LogPriority priority, const char *filename, int line, const char *fmt, ...) {
    time_t now;
    time(&now);
    char timeString[80];
    strftime(timeString, sizeof timeString, "%F %T", gmtime(&now));

    for (auto [file, handlerPriority] : g_logHandlers) {
        if (priority >= handlerPriority) {
            va_list args;
            va_start(args, fmt);
            if (filename != nullptr) {
                fprintf(file, "[%s] %s: %s:%d : ", timeString, priorityString(priority), basename(filename), line);
            } else {
                fprintf(file, "[%s] %s: ", timeString, priorityString(priority));
            }
            vfprintf(file, fmt, args);
            fprintf(file, "\n");
            va_end(args);
        }
    }
}

