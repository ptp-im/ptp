#include "Logger.h"

#ifdef ANDROID

#else
    CSLog g_imlog = CSLog(LOG_MODULE_IM);
#endif


