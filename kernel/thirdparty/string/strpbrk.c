#include <string.h>
#include <stdint.h>

char* strpbrk(const char* s, const char* b)
{
	s += strcspn(s, b);
	return *s ? (char*)(uintptr_t)s : 0;
}
