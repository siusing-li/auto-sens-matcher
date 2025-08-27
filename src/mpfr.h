#ifndef MPFR_H
#define MPFR_H

#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <mpfr.h>

// convert an mpfr to string, without trailing zeros
char* mpfr_to_str(mpfr_t *val, int precision);

#endif // MPFR_H