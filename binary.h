#ifndef BINARY_H
#define BINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <mpfr.h>
#include "global.h"

// after a move, previous values gets trickled down
    // the previous step is saved as the previous previous step
    // current state is saved before overwriting them, as the previous step for next iteration
void trickleDownPreviousState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high
);

// undo function
void undoCurrentState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high
);

// reset the loop for next iteration
void resetLowHighLoop(mpfr_t *current_number, char *input);

// translate mpfr to string
char* mpfr_to_str(mpfr_t *val, int precision);

// copy "new" value to clipboard
void copy_to_clipboard(const char *text);



#endif // BINARY_H