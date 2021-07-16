#ifndef LOGGING_H
#define LOGGING_H

#ifndef _TEST_ENV
#include <Arduino.h>
#endif
#include <cstdarg>
#include <cstdio>

class Logger
{
public:
        static size_t printf(const char *format, ...)
        {
#ifdef _SERIAL_LOG_VERBOSE
                char buffer[512];

                va_list args;
                va_start(args, format);
                vsnprintf(buffer, 512, format, args);
                va_end(args);

                return Serial.print(buffer);
#else
                return 0;
#endif
        }

        static size_t printfln(const char *format, ...)
        {
#ifdef _SERIAL_LOG_VERBOSE
                char buffer[512];

                va_list args;
                va_start(args, format);
                vsnprintf(buffer, 512, format, args);
                va_end(args);

                size_t r = Serial.print(buffer);
                return r + Serial.println();
#else
                return 0;
#endif
        }

        static size_t println(const char *str)
        {
#ifdef _SERIAL_LOG_VERBOSE
                return Serial.println(str);
#else
                return 0;
#endif
        }

        static size_t println(const String &str)
        {
#ifdef _SERIAL_LOG_VERBOSE
                return Serial.println(str);
#else
                return 0;
#endif
        }

        static size_t print(const char *str)
        {
#ifdef _SERIAL_LOG_VERBOSE
                return Serial.print(str);
#else
                return 0;
#endif
        }

        static size_t print(const String &str)
        {
#ifdef _SERIAL_LOG_VERBOSE
                return Serial.print(str);
#else
                return 0;
#endif
        }

        static size_t print(int n)
        {
#ifdef _SERIAL_LOG_VERBOSE
                return Serial.print(n);
#else
                return 0;
#endif
        }
};

using Log = Logger;

#endif // LOGGING_H
