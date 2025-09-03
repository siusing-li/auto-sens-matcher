#ifndef REVOLUTION_H
#define REVOLUTION_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <dinput.h>
#include <mpfr.h>
#include "panorama.h"
#include "hotkey.h"


int parseArgs(int *argc, char ***argv, int numArgs);

// move cursor "dx" and "dy" pixels relative to its current position
void moveMouseRelative(int dx, int dy);

// exit program
void exitRevolution(mpfr_t *sensitivity, mpfr_t *yaw, mpfr_t *ratio, mpfr_t *revolution, mpfr_t *increment, mpfr_t *total_counts, mpfr_t *total_counts_int, mpfr_t *total_counts_fractional, mpfr_t *step, mpfr_t *steps_needed, mpfr_t *fractional_sum, mpfr_t *fractional_sum_floor);

#endif // REVOLUTION_H