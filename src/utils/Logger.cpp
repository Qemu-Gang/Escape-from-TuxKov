#include "Logger.h"

#include <cstdio> // setbuf, fopen, fclose..
#include <cstdarg> // va_list, va_start, va_arg, va_end
#include <string.h>
#include <fstream>
//LMODE
#include "vmread/definitions.h"

#if (LMODE() != MODE_EXTERNAL())
#define LOGFILE_NAME "/tmp/tuxkov.log"
#endif

void Logger::Log( const char *format, ... ) {
    char buffer[4096];
    FILE *logFile;
#if (LMODE() == MODE_EXTERNAL())
    logFile = stdout;
#else
    static bool bFirst = true;

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
#endif

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 4096, format, args);
    fprintf(logFile, buffer);
    va_end(args);
    fflush(logFile);
#if (LMODE() != MODE_EXTERNAL())
    fclose(logFile);
#endif
}
