#ifndef LOGGER_H
#define LOGGER_H
#pragma once

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>

#define LOGGER(x) log4cplus::Logger::getInstance(LOG4CPLUS_TEXT({x}))

void init_logger() {
    log4cplus::initialize();
    log4cplus::SharedAppenderPtr appender_file(
                   new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT("Server") LOG4CPLUS_TEXT(".log"), 5 * 1024 * 1024, 5));
    log4cplus::BasicConfigurator config;
    appender_file->setName(LOG4CPLUS_TEXT("Server"));
    appender_file->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(LOG4CPLUS_TEXT("%D{%d-%m-%Y %H:%M:%S.%q} [%-5p] <%x> %c - %m%n"))));
    log4cplus::Logger::getRoot().addAppender(appender_file);
}

#endif // LOGGER_H
