#include <string>
#include <cstring>
#include <memory.h>
#include "ptp_crypto/memzero.h"

void memzero(void *s, size_t n)
{
	memset(s, 0, n);
}
