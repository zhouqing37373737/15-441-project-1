#ifndef LOGGER_H

#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"


enum level{
	DEBUG=1,
	INFO=2,
	WARNING=3,
	ERR=4,
};

enum output{
	TERMINAL,
	FILEIO,
};

FILE *default_logfd;
enum output default_out;
enum level default_level;
void init_logger(enum output out,enum level lev,char *log_file);
void logger(enum level lev,const char * format, ...);
void clear_logger();

#endif