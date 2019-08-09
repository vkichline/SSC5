#pragma once
#include <stdarg.h>


// Logging class, can write to serial and/or SD card
// Note: SD card TBD
//
//  Level         Printed Calls
//  LOG_VERBOSE   verbose(), debug(), info(), error()
//  LOG_DEBUG     debug(), info(), error()
//  LOG_INFO      info(), error()
//  LOG_ERROR     error()
//  LOG_NONE      none

#define LOG_NONE     0
#define LOG_ERROR    1
#define LOG_INFO     2
#define LOG_DEBUG    3
#define LOG_VERBOSE  4


class Log {
  public:
    Log() { }
    void init(long baud, int logLevel = LOG_INFO);
    void error(const char* format, ...);
    void error() ;
    void info(const char* format, ...);
    void info();
    void debug(const char* format, ...);
    void debug();
    void verbose(const char* format, ...);
    void verbose();

  private:
    void   printOutput(const char* format, va_list argp);
    int    _logLevel = LOG_INFO;
};

