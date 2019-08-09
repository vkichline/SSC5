#include "Log.h"
#include <HardwareSerial.h>
#include<stdio.h>

#define BUFFER_SIZE 256


void Log::init(long baud, int logLevel) {
  _logLevel = logLevel;
  Serial.begin(baud);
}

void Log::error(const char* fmt, ...) {
  if(_logLevel >= LOG_ERROR) {
    va_list argp;
    va_start(argp, fmt);
    printOutput(fmt, argp);
    va_end(argp);
  }
}
void Log::error() {
  if(_logLevel >= LOG_ERROR)
    Serial.println();
}

void Log::info(const char* fmt, ...) {
  if(_logLevel >= LOG_INFO) {
    va_list argp;
    va_start(argp, fmt);
    printOutput(fmt, argp);
    va_end(argp);
  }
}
  
void Log::info() {
  if(_logLevel >= LOG_INFO) {
    Serial.println();
  }
}

void Log::debug(const char* fmt, ...) {
  if(_logLevel >= LOG_DEBUG) {
    va_list argp;
    va_start(argp, fmt);
    printOutput(fmt, argp);
    va_end(argp);
  }
}

void Log::debug() {
  if(_logLevel >= LOG_DEBUG) {
    Serial.println(); 
  }
}

void Log::verbose(const char* fmt, ...) {
  if(_logLevel >= LOG_VERBOSE) {
    va_list argp;
    va_start(argp, fmt);
    printOutput(fmt, argp);
    va_end(argp);
  }
}

void Log::verbose() {
  if(_logLevel >= LOG_VERBOSE) {
    Serial.println();
  }
}

void Log::printOutput(const char* format, va_list argp) {
  char buffer[BUFFER_SIZE];
  vsnprintf(buffer, BUFFER_SIZE, format, argp);
  Serial.print(buffer);
}

