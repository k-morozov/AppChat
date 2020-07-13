#include "logger.h"


void init_logger() {
    log4cplus::initialize();

    log4cplus::SharedAppenderPtr console_appender(new log4cplus::ConsoleAppender());
    console_appender->setName(LOG4CPLUS_TEXT("Console_appender"));
    log4cplus::Logger::getRoot().addAppender(console_appender);

    log4cplus::SharedAppenderPtr file_appender(new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT("Server.log"), 5 * 1024 * 1024, 5));
    file_appender->setName(LOG4CPLUS_TEXT("File_appender"));
    file_appender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(LOG4CPLUS_TEXT("%D{%d-%m-%Y %H:%M:%S.%q} [%-5p] <%x> %c - %m%n"))));
    log4cplus::Logger::getRoot().addAppender(file_appender);
}
