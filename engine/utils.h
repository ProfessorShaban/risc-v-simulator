
#include <stddef.h>

#ifdef FROM_CPP

extern "C" char * strrev (char *str);
extern "C" void copy_string (char *dest, const char *src);
extern "C" void copy_string_len (char *dest, const char *src, int max);
extern "C" void append_string (char *dest, const char *src);
extern "C" int stricmp (const char *str1, const char *str2);
extern "C" size_t strlen (const char *str);
extern "C" unsigned long long sign_extend (unsigned long long num, int bits);

#endif

#ifndef FROM_CPP

char * strrev (char *str);
void copy_string (char *dest, const char *src);
void copy_string_len (char *dest, const char *src, int max);
void append_string (char *dest, const char *src);
int stricmp (const char *str1, const char *str2);
size_t strlen (const char *str);
unsigned long long sign_extend (unsigned long long num, int bits);

#endif
