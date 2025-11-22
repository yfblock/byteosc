#pragma once

#include "stdint.h"

/* note: may already be defined */
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef offsetof
#define offsetof(TYPE, FIELD) ((size_t) &((TYPE *)0)->FIELD)
#endif
