#include "zzutil.h"

char *zz_gen_fullpath(const char *base, const char *sub)
{
	size_t size = strlen(base) + strlen(sub) + 1;
	char *p = (char *)malloc(size);
	if (p == NULL)
		return NULL;
	strcpy(p, base);
	strcat(p, sub);
	return p;
}


void zz_tmpname(char *str, size_t size)
{
	size_t i = 0;
	while(i < size - 1)
	{
#pragma warning(disable:4244)
		char c = (float) rand() / RAND_MAX * CHAR_MAX;
#pragma warning(default:4244)
		if (isalpha(c) || isdigit(c))
		{
			str[i] = c;
			++i;
		}
	}
	str[size - 1] = '\0';
}