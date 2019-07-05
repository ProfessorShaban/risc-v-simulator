

#include <ctype.h>

#include "utils.h"


char * strrev (char *str)
{
	unsigned long len = strlen(str);
	for (unsigned long i = 0; i < len / 2; i++) {
		char temp = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = temp;
	}
    return str;
}

void copy_string (char *dest, const char *src)
{
	while ((*dest++ = *src++));
}

void copy_string_len (char *dest, const char *src, int max)
{
    int count = 0;
    while (count++ < max && (*dest++ = *src++));
}

void append_string (char *dest, const char *src)
{
	while (*dest != 0)
		dest ++;
	while ((*dest++ = *src++));
}

int stricmp (const char *str1, const char *str2)
{
	unsigned long len1 = strlen(str1);
	unsigned long len2 = strlen(str2);

    for (unsigned long i = 0; i <= len1 && i <= len2; i++)
        if (tolower (str1[i]) > tolower (str2[i]))
			return 1;
        else if (tolower (str1[i]) < tolower (str2[i]))
            return -1;

	return 0;
}

size_t strlen (const char *str)
{
	int len = 0;

	while (str[len] != 0)
		len++;

	return len;
}

unsigned long long sign_extend (unsigned long long num, int bits)
{
    int sign = (num >> (bits - 1)) & 1;
    if (sign == 1)
    {
        unsigned long long extended = (unsigned long long) -1;
        for (int i = 0; i < bits; i++) {
            extended = extended & ~((unsigned long long) 1 << i);
        }
        num = num | extended;
    }
    return num;
}
