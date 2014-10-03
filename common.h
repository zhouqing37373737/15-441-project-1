
#ifndef COMMON_H

#define COMMON_H

#define MAXLINESIZE 255
#define ECHO_PORT 9999
#define BUF_SIZE 4096

char *str_tolower(char *str);
char *str_truncate(char *str);
int str_loosecompare(char *str1,char *str2);

#endif