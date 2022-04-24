// An abbreviated version of the u-boot exports.h for C++.
// The u-boot version is incompatible with C++.

#ifndef __EXPORTS_HPP__
#define __EXPORTS_HPP__

#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

void app_startup(char * const *);
unsigned long get_version(void);
#ifdef __cplusplus
int  getc(FILE *);
int putc(const int, FILE *);
#else
int  getc(void);
int putc(const int, void *);
#endif
int  tstc(void);
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
void *malloc(unsigned size);
void free(void *addr);

#ifdef __cplusplus
};
#endif

#endif	/* __EXPORTS_HPP__ */
