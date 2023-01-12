#ifndef Logger_H
#define Logger_H
#include <string>
#include <sstream>
#include <cstring>

#ifdef ANDROID

class Logger {
public:
    Logger();
    void init(std::string path);
    static void e(const char *message, ...) __attribute__((format (printf, 1, 2)));
    static void w(const char *message, ...) __attribute__((format (printf, 1, 2)));
    static void i(const char *message, ...) __attribute__((format (printf, 1, 2)));
    static void d(const char *message, ...) __attribute__((format (printf, 1, 2)));

    static Logger &getInstance();

private:
    FILE *logFile = nullptr;
    pthread_mutex_t mutex;
};


#ifndef DEBUG_D
    #define DEBUG_E Logger::getInstance().e
    #define DEBUG_W Logger::getInstance().w
    #define DEBUG_D Logger::getInstance().d
#endif

#else
    #include "slog_api.h"
    #define LOG_MODULE_IM         "IM"

    extern CSLog g_imlog;
    #define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
    #if defined(_WIN32) || defined(_WIN64)
        #define DEBUG_I(fmt, ...)  g_imlog.Info("<%s>\t<%d>\t<%s>,"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
        #define DEBUG_D(fmt, ...)  g_imlog.Debug("<%s>\t<%d>\t<%s>,"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
        #define DEBUG_E(fmt, ...)  g_imlog.Error("<%s>\t<%d>\t<%s>,"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #else
        #define DEBUG_I(fmt, args...)  g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
        #define DEBUG_D(fmt, args...)  g_imlog.Debug("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
        #define DEBUG_E(fmt, args...)  g_imlog.Error("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
    #endif
#endif

#endif
