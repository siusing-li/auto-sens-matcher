#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <mpfr.h>
#include <windows.h>
#include "global.h"

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_RESET   "\033[0m"

// void set_console_color(WORD color);
// void reset_console_color();

// checks string is valid float format
bool is_valid_float_string(char *str);

// Ask the user for a float value with a message
void askFloatUserInput(char **prompt, mpfr_t *dest);

// Remove the newline character from string user input
void trim_newline(char *str);

// Remove trailing and leading whitespace from string user input
void trim_whitespace(char *str);

// Ask the user for string input, validate it against a list, and store it in *input
void askStringUserInput(
    char **prompt,            // Prompt message
    char *input,             // Pointer to store user input
    char **valid_inputs_list,     // List of valid input strings
    const size_t list_size            // Maximum length of the input string
);

// ask user, set min, base, and max number
void askInitialValues(mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *base_number);

// copy string value to clipboard
void copy_to_clipboard(const char *text);

// print all current state of numbers
void printCurrentNumbers(mpfr_t *low_number, mpfr_t *high_number, mpfr_t *current_number);

#endif // STREAM_H