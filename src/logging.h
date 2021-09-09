#pragma once

#include <SDL_log.h>

enum LogPriority { DEBUG,
                   INFO,
                   WARN,
                   ERROR };

extern void logInit();
extern void log(LogPriority priority, const char *filename, int line, const char *fmt, ...);

#define LOG_DEBUG(fmt, args...) log(LogPriority::DEBUG, __FILE__, __LINE__, fmt, ##args)
#define LOG_INFO(fmt, args...) log(LogPriority::INFO, __FILE__, __LINE__, fmt, ##args)
#define LOG_WARN(fmt, args...) log(LogPriority::WARN, __FILE__, __LINE__, fmt, ##args)
#define LOG_ERROR(fmt, args...) log(LogPriority::ERROR, __FILE__, __LINE__, fmt, ##args)

