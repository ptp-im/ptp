#include <string>
#include <cstring>
#include <memory.h>
#include "memzero.h"

void memzero(void *s, size_t n)
{
	memset(s, 0, n);
}
