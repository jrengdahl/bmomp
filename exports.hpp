#ifndef __EXPORTS_HPP__
#define __EXPORTS_HPP__

#include <stdarg.h>

extern "C"
{
void app_startup(char * const *);
unsigned long get_version(void);
int  getc(void);
int  tstc(void);
int putc(const int, void *);
int puts(const char*);
int printf(const char* fmt, ...);
void __udelay(unsigned long);
unsigned long get_timer(unsigned long);
int vprintf(const char *, va_list);
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);
char *env_get(const char *name);
int env_set(const char *varname, const char *value);
long simple_strtol(const char *cp, char **endp, unsigned int base);
int strcmp(const char *cs, const char *ct);
unsigned long ustrtoul(const char *cp, char **endp, unsigned int base);
unsigned long long ustrtoull(const char *cp, char **endp, unsigned int base);
};

#endif	/* __EXPORTS_HPP__ */
