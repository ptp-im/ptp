/*
 * This is the source code of tgnet library v. 1.1
 * It is licensed under GNU GPL v. 2 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright Nikolai Kudashov, 2015-2018.
 */

#ifndef Logger_H
#define Logger_H
#include <string>

#ifdef ANDROID
#include <android/log.h>
#endif
class Logger {
public:
    Logger();
    void init(std::string path);
    static void e(const char *message, ...) __attribute__((format (printf, 1, 2)));
    static void w(const char *message, ...) __attribute__((format (printf, 1, 2)));
    static void d(const char *message, ...) __attribute__((format (printf, 1, 2)));

    static Logger &getInstance();

private:
    FILE *logFile = nullptr;
    pthread_mutex_t mutex;
};

#ifdef LOGS_ENABLED_PTP

#else
extern bool LOGS_ENABLED_PTP;

#endif





#ifdef ANDROID

#else
#define DEBUG_E Logger::getInstance().e
#define DEBUG_W Logger::getInstance().w
#define DEBUG_D Logger::getInstance().d
#endif

#endif
