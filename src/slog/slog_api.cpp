//
//  slog_api.cpp
//  slog
//
//  Created by luoning on 15-1-6.
//  Copyright (c) 2015年 luoning. All rights reserved.
//
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "slog_api.h"
#include <cstdarg>
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"
#include <log4cxx/helpers/properties.h>
#include <log4cxx/logmanager.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

#define MAX_LOG_LENGTH   1024 * 10

class CSLogObject
{
public:
    CSLogObject(const char* module_name, int delay = WATCH_DELAY_TIME) {}
    virtual ~CSLogObject() {}
    
    virtual void Trace(const char* loginfo) {}
    virtual void Debug(const char* loginfo) {}
    virtual void Info(const char* loginfo) {}
    virtual void Warn(const char* loginfo) {}
    virtual void Error(const char* loginfo) {}
    virtual void Fatal(const char* loginfo) {}
};

class CLog4CXX : public CSLogObject
{
public:
    CLog4CXX(const char* module_name, int delay = WATCH_DELAY_TIME);
    virtual ~CLog4CXX();
    
    void Trace(const char* loginfo);
    void Debug(const char* loginfo);
    void Info(const char* loginfo);
    void Warn(const char* loginfo);
    void Error(const char* loginfo);
    void Fatal(const char* loginfo);
private:
    LoggerPtr m_logger;
};

CLog4CXX::CLog4CXX(const char* module_name, int delay) : CSLogObject(module_name, delay)
{
    PropertyConfigurator::configureAndWatch("log4cxx.properties", delay);
    m_logger = Logger::getLogger(module_name);
}

CLog4CXX::~CLog4CXX()
{
}

void CLog4CXX::Trace(const char *loginfo)
{
    m_logger->trace(loginfo);
    //LOG4CXX_TRACE(m_logger, loginfo);
}

void CLog4CXX::Debug(const char *loginfo)
{
    m_logger->debug(loginfo);
}

void CLog4CXX::Info(const char *loginfo)
{
    m_logger->info(loginfo);
}

void CLog4CXX::Warn(const char *loginfo)
{
    m_logger->warn(loginfo);
}

void CLog4CXX::Error(const char *loginfo)
{
    m_logger->error(loginfo);
}

void CLog4CXX::Fatal(const char *loginfo)
{
    m_logger->fatal(loginfo);
}

CSLog::CSLog(const char* module_name, int delay)
{
    m_log = new CLog4CXX(module_name, delay);
}

CSLog::~CSLog()
{
    delete m_log;
}

void CSLog::Trace(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer), format, args);
    va_end(args);
    m_log->Trace(szBuffer);
}

void CSLog::Debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer) , format, args);
    va_end(args);
    m_log->Debug(szBuffer);
}

void CSLog::Info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer), format, args);
    va_end(args);
    m_log->Info(szBuffer);
}

void CSLog::Warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer), format, args);
    va_end(args);
    m_log->Warn(szBuffer);
}

void CSLog::Error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer), format, args);
    va_end(args);
    m_log->Error(szBuffer);
}

void CSLog::Fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char szBuffer[MAX_LOG_LENGTH];
    vsnprintf(szBuffer, sizeof(szBuffer), format, args);
    va_end(args);
    m_log->Fatal(szBuffer);
}

void slog_set_append(bool enableConsole,bool isDebug,bool enableRollingFileAppender,const std::string &fileName){
    Properties props;
    std::string rootLogger;
    if(isDebug){
        rootLogger.append("DEBUG, ");
    }else{
        rootLogger.append("INFO, ");
    }
    if(enableConsole){
        rootLogger.append("stdout");
    }

    if(enableRollingFileAppender){
        rootLogger.append(", default");
    }
    props.put(LOG4CXX_STR("log4j.rootLogger"),LOG4CXX_STR(rootLogger));
    props.put(LOG4CXX_STR("log4j.additivity.org.apache"), LOG4CXX_STR("false"));
    if(enableConsole){
        props.put(LOG4CXX_STR("log4j.appender.stdout"), LOG4CXX_STR("org.apache.log4j.ConsoleAppender"));
        props.put(LOG4CXX_STR("log4j.appender.stdout.layout"), LOG4CXX_STR("org.apache.log4j.PatternLayout"));
        props.put(LOG4CXX_STR("log4j.appender.stdout.layout.ConversionPattern"), LOG4CXX_STR("%d{HH:mm:ss,SSS} [ %t ] [ %-5p ] - %m%n"));
    }
    if(enableRollingFileAppender){
        std::string Threshold = isDebug ? "DEBUG": "INFO";
        props.put(LOG4CXX_STR("log4j.appender.default"), LOG4CXX_STR("org.apache.log4j.RollingFileAppender"));
        props.put(LOG4CXX_STR("log4j.appender.default.File"), LOG4CXX_STR(fileName));
        props.put(LOG4CXX_STR("log4j.appender.default.MaxFileSize"), LOG4CXX_STR("10MB"));
        props.put(LOG4CXX_STR("log4j.appender.default.MaxBackupIndex"), LOG4CXX_STR("12"));
        props.put(LOG4CXX_STR("log4j.appender.default.Append"), LOG4CXX_STR("true"));
        props.put(LOG4CXX_STR("log4j.appender.default.Threshold"), LOG4CXX_STR(Threshold));
        props.put(LOG4CXX_STR("log4j.appender.default.layout"), LOG4CXX_STR("org.apache.log4j.PatternLayout"));
        props.put(LOG4CXX_STR("log4j.appender.default.layout.ConversionPattern"), LOG4CXX_STR("%d [ %-5p ] [ %t ] - %m%n"));
    }
    PropertyConfigurator::configure(props);

}


#pragma GCC diagnostic pop
