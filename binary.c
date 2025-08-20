#include "binary.h"
#define BUFFER_SIZE 120
#define BIT_PRECISION 480 // 120 * 4 bits
#define BASE_TEN 10

// input: 
//     base sens
//     too low
//     too high
//     perfect
// output
//     current sens to test
//     perfect sens

int main() {
    // mpfr_t hi;
    // mpfr_init2(hi, BIT_PRECISION);
    // mpfr_set_d(hi, 100, MPFR_RNDN);
    // for (int i = 0; i < 120; i++) {
    //     mpfr_div_ui(hi, hi, 2, MPFR_RNDN);
    //     char *mpfr_str = mpfr_to_str(&hi, BIT_PRECISION);
    //     mpfr_t hi2;
    //     mpfr_init2(hi2, BIT_PRECISION);
    //     mpfr_set_str(hi2, mpfr_str, 10, MPFR_RNDN);
    //     mpfr_printf("%d\n%.110Rf\n", i, hi2);
    //     if (mpfr_cmp(hi, hi2) != 0) {
    //         mpfr_printf("WRONG\n%.100Rf\n", hi2);
    //     }
    //     mpfr_printf("%s\n", mpfr_str);
    // }

    mpfr_t base_number; // number to start with
    mpfr_t current_number; // current number
    mpfr_t low_number; // number that is too low in range
    mpfr_t high_number; // number that is too high in range
    char input[BUFFER_SIZE]; // user input

    // initialize mpfr values
    mpfr_inits2(BIT_PRECISION, base_number, current_number, low_number, high_number, NULL);

    askInitialValues(&base_number, &low_number, &high_number, &current_number);

    // high or low input prompt
    char *high_low_prompt = "Please specify whether current value was \"low\" (1) or \"high\" (h) or \"perfect\" (p). You can also \"undo\" or \"exit\": ";
    char *valid_inputs_list[] = {"low", "l", "high", "h", "perfect", "p", "undo", "u", "exit"};

    // save current values in case of undo
    mpfr_t prev_curr, prev_prev_curr;
    mpfr_t prev_low, prev_prev_low;
    mpfr_t prev_high, prev_prev_high;
    mpfr_inits2(BIT_PRECISION, prev_curr, prev_low, prev_high, prev_prev_curr, prev_prev_low, prev_prev_high, NULL);
    mpfr_set(prev_curr, current_number, MPFR_RNDN);
    mpfr_set(prev_low, low_number, MPFR_RNDN);
    mpfr_set(prev_high, high_number, MPFR_RNDN);
    mpfr_set(prev_prev_curr, current_number, MPFR_RNDN);
    mpfr_set(prev_prev_low, low_number, MPFR_RNDN);
    mpfr_set(prev_prev_high, high_number, MPFR_RNDN);

    // ask user if number is too low or too high, adjust accordingly
    while (1) {
        askStringUserInput(&high_low_prompt, input, valid_inputs_list, 9);
        
        if (strcmp(input, "l") == 0 || strcmp(input, "low") == 0) {
            printf("Too low, adjusting value:\n");
            trickleDownPreviousState(
                &current_number, &low_number, &high_number, 
                &prev_curr, &prev_low, &prev_high, 
                &prev_prev_curr, &prev_prev_low, &prev_prev_high
            );
            // low_number = current_number
            mpfr_set(low_number, current_number, MPFR_RNDN);
        } else if (strcmp(input, "h") == 0 || strcmp(input, "high") == 0) {
            printf("Too high, adjusting value:\n");

            trickleDownPreviousState(
                &current_number, &low_number, &high_number, 
                &prev_curr, &prev_low, &prev_high, 
                &prev_prev_curr, &prev_prev_low, &prev_prev_high
            );

            // high_number = current_number
            mpfr_set(high_number, current_number, MPFR_RNDN);
        } else if (strcmp(input, "p") == 0 
                    || strcmp(input, "perfect") == 0  
                    || strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "undo") == 0) {
            // set numbers to previous state
            printf("Undo-ing. Reverting to previous numbers:\n");
            printf("Undo-ing only saves last two instances.\n");

            undoCurrentState(
                &current_number, &low_number, &high_number, 
                &prev_curr, &prev_low, &prev_high, 
                &prev_prev_curr, &prev_prev_low, &prev_prev_high
            );

            // print the change, reset the loop
            mpfr_printf("Low Number: %s\n", mpfr_to_str(&low_number, BIT_PRECISION));
            mpfr_printf("High Number: %s\n", mpfr_to_str(&high_number, BIT_PRECISION));
            mpfr_printf(COLOR_GREEN "Current Number: %s" COLOR_RESET "\n", mpfr_to_str(&current_number, BIT_PRECISION));

            copy_to_clipboard(mpfr_to_str(&current_number, BIT_PRECISION));

            memset(input, '\0', BUFFER_SIZE);
            continue;
        } else {
            return -1;
        }

        // adjust the current value according to user's request
        // current_number = (high_number - low_number) / 2 + low_number
        mpfr_sub(current_number, high_number, low_number, MPFR_RNDN);
        mpfr_div_ui(current_number, current_number, 2, MPFR_RNDN);
        mpfr_add(current_number, current_number, low_number, MPFR_RNDN);

        resetLowHighLoop(&current_number, input);

        if (mpfr_eq(current_number, low_number, BIT_PRECISION) && mpfr_eq(current_number, high_number, BIT_PRECISION)) {
            printf("Your number is accurate up to ~120 digits.\n");
            break;
        }
    }

    mpfr_printf("Low Number: %s\n", mpfr_to_str(&low_number, BIT_PRECISION));
    mpfr_printf("High Number: %s\n", mpfr_to_str(&high_number, BIT_PRECISION));
    mpfr_printf(COLOR_GREEN "YOUR Number: %s" COLOR_RESET "\n", mpfr_to_str(&current_number, BIT_PRECISION));

    mpfr_clears(base_number, current_number, low_number, high_number, prev_curr, prev_low, prev_high, prev_prev_curr, prev_prev_low, prev_prev_high, NULL);
    mpfr_free_cache();
    return 0;
}

bool is_valid_float_string(char *str) {
    int i = 0;
    bool has_digit = false, has_dot = false;

    // Optional leading sign
    if (str[i] == '+' || str[i] == '-') {
        i++;
    }

    while (str[i]) {
        if (isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else {
            return false;
        }
        i++;
    }

    return has_digit;
}

void askFloatUserInput(char **prompt, mpfr_t *dest) {
    mpfr_t value;
    mpfr_init2(value, BIT_PRECISION);
    char buffer[BUFFER_SIZE];

    while (1) {
        printf(COLOR_BLUE "%s" COLOR_RESET, *prompt);

        if (fgets(buffer, BUFFER_SIZE, stdin)) {
            trim_newline(buffer);
            trim_whitespace(buffer);
            if (strcmp(buffer, "exit") == 0) {
                exit(0);
            }
            if (is_valid_float_string(buffer)) {
                mpfr_set_str(value, buffer, BASE_TEN, MPFR_RNDN);
                mpfr_set(*dest, value, MPFR_RNDN);
                return;
            }
        }
        printf("Try again with a valid value. \n");
    }

}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
}

void trim_whitespace(char *str) {
    char *start = str;
    char *end;

    // Move start to first non-whitespace character
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        str[0] = '\0';
        
        return;
    }

    // Find end of string
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    *(end + 1) = '\0';

    // Move trimmed string to beginning
    memmove(str, start, end - start + 2); // +1 for null terminator
}

char* mpfr_to_str(mpfr_t *val, int precision)  {
    size_t buf_size = precision + 8; // + 8 for dot
    char *str = malloc(buf_size);
    if (!str) return NULL;

    // Convert to string with fixed-point format and desired precision
    mpfr_sprintf(str, "%.*Rf", BUFFER_SIZE, *val);

    // Trim trailing zeros manually (but preserve at least one digit after decimal point)
    char *dot = strchr(str, '.');
    if (dot != NULL) {
        char *end = str + strlen(str) - 1;
        while (end > dot && *end == '0') {
            *end = '\0';
            --end;
        }
        // If the last character is a dot, remove it too
        if (*end == '.') {
            *end = '\0';
        }
    }

    return str;
}

void askStringUserInput(char **prompt, char *input, char **valid_inputs_list, const size_t list_size) {
    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    while (1) {
        printf(COLOR_BLUE "%s" COLOR_RESET, *prompt);
        if (fgets(buffer, BUFFER_SIZE, stdin)) {

            trim_newline(buffer);
            trim_whitespace(buffer);
            
            for (int i = 0; buffer[i]; i++) {
                buffer[i] = tolower(buffer[i]);
            }

            for (int i = 0; i < list_size; i++) {
                if (strcmp(buffer, valid_inputs_list[i]) == 0) {
                    strcpy(input, buffer);

                    free(buffer);
                    buffer = NULL;

                    return;
                }
            }
        }
        printf("Try again with a valid value. \n");
        memset(buffer, '\0', BUFFER_SIZE);
    }
    free(buffer);
    buffer = NULL;
}

void askInitialValues(mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *base_number) {
    // get base number
    char *base_number_prompt = "Please enter a base number/decimal value: ";
    askFloatUserInput(&base_number_prompt, base_number);
    mpfr_printf("Base number set to: %s\n", mpfr_to_str(base_number, BIT_PRECISION));

    // setting default values
    mpfr_set(*current_number, *base_number, MPFR_RNDN);
    mpfr_set_d(*low_number, 0.0, MPFR_RNDN);
    mpfr_set(*high_number, *base_number, MPFR_RNDN);
    
    // get min number
    char *min_number_prompt = "Please enter a MIN number/decimal value: ";
    while (1) {
        askFloatUserInput(&min_number_prompt, low_number);
        if (mpfr_cmp(*low_number, *base_number) <= 0) {
            break;
        }
        printf("Less than or equal to your base number.\n");
    }
    mpfr_printf("MIN number set to: %s\n", mpfr_to_str(low_number, BIT_PRECISION));
    
    // get max number
    char *max_number_prompt = "Please enter a MAX number/decimal value: ";
    while (1) {
        askFloatUserInput(&max_number_prompt, high_number);
        if (mpfr_cmp(*high_number, *base_number) >= 0) {
            break;
        }
        printf("Greater than or equal to your base number.\n");
    }
    mpfr_printf("MAX number set to: %s\n", mpfr_to_str(high_number, BIT_PRECISION));
}

void trickleDownPreviousState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high
) {
        // mpfr_printf("before\n");
        // mpfr_printf("prev prev Number: %.100Rf\n", *prev_prev_curr);
        // mpfr_printf("prev prev Low Number: %.100Rf\n", *prev_prev_low);
        // mpfr_printf("prev prev High Number: %.100Rf\n", *prev_prev_high);
        // mpfr_printf("prev Number: %.100Rf\n", *prev_curr);
        // mpfr_printf("prev Low Number: %.100Rf\n", *prev_low);
        // mpfr_printf("prev High Number: %.100Rf\n", *prev_high);

    // TODO: print reverting (high or low), undo-ing again reverts (high or low)

    mpfr_set(*prev_prev_curr, *prev_curr, MPFR_RNDN);
    mpfr_set(*prev_prev_low, *prev_low, MPFR_RNDN);
    mpfr_set(*prev_prev_high, *prev_high, MPFR_RNDN);
    mpfr_set(*prev_curr, *current_number, MPFR_RNDN);
    mpfr_set(*prev_low, *low_number, MPFR_RNDN);
    mpfr_set(*prev_high, *high_number, MPFR_RNDN);
        // mpfr_printf("after\n");
        // mpfr_printf("prev prev Number: %.100Rf\n", *prev_prev_curr);
        // mpfr_printf("prev prev Low Number: %.100Rf\n", *prev_prev_low);
        // mpfr_printf("prev prev High Number: %.100Rf\n", *prev_prev_high);
        // mpfr_printf("prev Number: %.100Rf\n", *prev_curr);
        // mpfr_printf("prev Low Number: %.100Rf\n", *prev_low);
        // mpfr_printf("prev High Number: %.100Rf\n", *prev_high);
}

void undoCurrentState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high
) {

    mpfr_set(*current_number, *prev_curr, MPFR_RNDN);
    mpfr_set(*low_number, *prev_low, MPFR_RNDN);
    mpfr_set(*high_number, *prev_high, MPFR_RNDN);
    mpfr_set(*prev_curr, *prev_prev_curr, MPFR_RNDN);
    mpfr_set(*prev_low, *prev_prev_low, MPFR_RNDN);
    mpfr_set(*prev_high, *prev_prev_high, MPFR_RNDN);
}

void resetLowHighLoop(mpfr_t *current_number, char *input) {
    // reset the loop, part 1
    // print current value
    mpfr_printf(COLOR_GREEN "Current Number: %s" COLOR_RESET, mpfr_to_str(current_number, BIT_PRECISION));
    // mpfr_printf(COLOR_GREEN " Current Number: %.120Rf" COLOR_RESET, *current_number);
    // mpfr_printf("Low Number: %.100Rf\n", low_number);
    // mpfr_printf("High Number: %.100Rf\n", high_number);
    
    copy_to_clipboard(mpfr_to_str(current_number, BIT_PRECISION));
    printf(" (Saved to clipboard.)\n");

    // reset the loop, part 2
    // clear input string
    memset(input, '\0', BUFFER_SIZE);
}

void copy_to_clipboard(const char *text) {
    const size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), text, len);
    GlobalUnlock(hMem);

    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}