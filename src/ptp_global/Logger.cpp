#include "Logger.h"

#ifdef ANDROID

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef ANDROID
#include <android/log.h>
#endif

#ifdef DEBUG_VERSION
bool LOGS_ENABLED = true;
#else
bool LOGS_ENABLED = false;
#endif

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    pthread_mutex_init(&mutex, NULL);
}

void Logger::init(std::string path) {
    pthread_mutex_lock(&mutex);
    if (path.size() > 0 && logFile == nullptr) {
        logFile = fopen(path.c_str(), "w");
    }
    pthread_mutex_unlock(&mutex);
}

void Logger::e(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, "tgnet", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#else
    printf("%d-%d %02d:%02d:%02d error: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    vprintf(message, argptr);
    printf("\n");
    fflush(stdout);
    va_end(argptr);
    va_start(argptr, message);
#endif
    FILE *logFile = getInstance().logFile;
    if (logFile) {
        fprintf(logFile, "%d-%d %02d:%02d:%02d error: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        vfprintf(logFile, message, argptr);
        fprintf(logFile, "\n");
        fflush(logFile);
    }

    va_end(argptr);
}


void Logger::i(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_WARN, "tgnet", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#else
    printf("%d-%d %02d:%02d:%02d warning: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    vprintf(message, argptr);
    printf("\n");
    fflush(stdout);
    va_end(argptr);
    va_start(argptr, message);
#endif
    FILE *logFile = getInstance().logFile;
    if (logFile) {
        fprintf(logFile, "%d-%d %02d:%02d:%02d info: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        vfprintf(logFile, message, argptr);
        fprintf(logFile, "\n");
        fflush(logFile);
    }

    va_end(argptr);
}

void Logger::w(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_WARN, "tgnet", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#else
    printf("%d-%d %02d:%02d:%02d warning: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    vprintf(message, argptr);
    printf("\n");
    fflush(stdout);
    va_end(argptr);
    va_start(argptr, message);
#endif
    FILE *logFile = getInstance().logFile;
    if (logFile) {
        fprintf(logFile, "%d-%d %02d:%02d:%02d warning: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        vfprintf(logFile, message, argptr);
        fprintf(logFile, "\n");
        fflush(logFile);
    }

    va_end(argptr);
}

void Logger::d(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, "tgnet", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#else
    printf("%d-%d %02d:%02d:%02d debug: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    vprintf(message, argptr);
    printf("\n");
    fflush(stdout);
    va_end(argptr);
    va_start(argptr, message);
#endif
    FILE *logFile = getInstance().logFile;
    if (logFile) {
        fprintf(logFile, "%d-%d %02d:%02d:%02d debug: ", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        vfprintf(logFile, message, argptr);
        fprintf(logFile, "\n");
        fflush(logFile);
    }

    va_end(argptr);
}

#else
    CSLog g_imlog = CSLog(LOG_MODULE_IM);
#endif


