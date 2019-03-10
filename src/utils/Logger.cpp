#include "Logger.h"

#include <cstdio> // setbuf, fopen, fclose..
#include <cstdarg> // va_list, va_start, va_arg, va_end
#include <string.h>
#include <fstream>

#define LOGFILE_NAME "/tmp/apex.log"

void Logger::Log( const char *format, ... ) {
    char buffer[4096];
    static bool bFirst = true;
    FILE *logFile;

    if ( bFirst ) {
        logFile = fopen(LOGFILE_NAME, "w"); // create new log
        bFirst = false;
        if (logFile)
            fprintf(logFile, "--Start of log--\n");
    } else {
        logFile = fopen(LOGFILE_NAME, "a"); // append to log
    }

    if (!logFile)
        return;

    setbuf( logFile, NULL ); // Turn off buffered I/O, decreases performance but if crash occurs, no unflushed buffer.
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 4096, format, args);
    fprintf(logFile, buffer);
    va_end(args);
    fclose(logFile);
}
