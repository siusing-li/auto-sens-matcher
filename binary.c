#include "binary.h"
#include "global.h"
#include "mpfr.h"
#include "input.h"
#include "revolution.h"

// input: 
//     base sens
//     too low
//     too high
//     perfect
// output
//     current sens to test
//     perfect sens

int main() {
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

// after undoing a change, move previous values up to reflect new current state
void trickleDownPreviousState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, 
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, 
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high
) {
    // TODO: print reverting (high or low), undo-ing again reverts (high or low)

    mpfr_set(*prev_prev_curr, *prev_curr, MPFR_RNDN);
    mpfr_set(*prev_prev_low, *prev_low, MPFR_RNDN);
    mpfr_set(*prev_prev_high, *prev_high, MPFR_RNDN);
    mpfr_set(*prev_curr, *current_number, MPFR_RNDN);
    mpfr_set(*prev_low, *low_number, MPFR_RNDN);
    mpfr_set(*prev_high, *high_number, MPFR_RNDN);
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