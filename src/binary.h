#ifndef BINARY_H
#define BINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <mpfr.h>
#include "global.h"

// print high low prompt
void printHighLowPrompt(char **high_low_prompt);

// after a move, previous values gets trickled down
    // the previous step is saved as the previous previous step
    // current state is saved before overwriting them, as the previous step for next iteration
void trickleDownPreviousState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio
);

// undo function
void undoCurrentState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio
);

// reset the loop for next iteration
void resetLowHighLoop(mpfr_t * prev_number, mpfr_t *current_number, char *input);

// translate mpfr to string
char* mpfr_to_str(mpfr_t *val, int precision);

// exit program
void exitProgram(
    mpfr_t *base_number, mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio,
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio,
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio,
    mpfr_t *tmp_ratio);

#endif // BINARY_H