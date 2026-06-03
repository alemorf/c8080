#pragma once

#ifdef ARCH_MICRO80_COLOR
#include "arch_micro80.h"
#endif

#ifdef ARCH_VT100
#include "arch_vt100.h"
#endif

// По умолчанию этих возможностей нет
#ifndef DISABLE_NC_AUTOSTART
#define DISABLE_NC_AUTOSTART
#endif

#ifndef ENABLE_NC_AUTOSTART
#define ENABLE_NC_AUTOSTART
#endif
