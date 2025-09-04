#include "binary.h"
#include "mpfr.h"
#include "stream.h"
#include "revolution.h"
#include "hotkey.h"

// cm/360 = 2.54 * 360 / (increment * CPI)
// different fov = turn_2 : inputs.turn_1 * Math.tan(inputs.fov_1 / 2 / 180 * Math.PI) / Math.tan(inputs.fov_2 / 2 / 180 * Math.PI)

// input:
//    base sens
//    min sens
//    max sens
//    too low
//    too high
//    perfect
// output:
//    current sens to test
//    perfect sens

int main()
{
    mpfr_t base_number;      // number to start with
    mpfr_t current_number;   // current number
    mpfr_t low_number;       // number that is too low in range
    mpfr_t high_number;      // number that is too high in range
    mpfr_t ratio;            // ratio of current number to base number
    char input[BUFFER_SIZE]; // user input

    // initialize mpfr values
    mpfr_inits2(BIT_PRECISION, base_number, current_number, low_number, high_number, ratio, NULL);

    askInitialValues(&base_number, &low_number, &high_number, &current_number, &ratio);

    // high or low input prompt
    char *high_low_prompt = "Hotkeys: \"Do a 360\" (Alt + Insert). \"Higher\" (Alt + Shift + Up), \"Lower\" (Alt + Shift + Down), \"Perfect\" (Alt + Shift + '\\'). \"Undo\" (Alt + Shift + Backspace) or \"Exit\" (Alt + Shift + '\\')\nSpecify whether current value should be higher, lower, or perfect. You can also undo or exit: \n";
    char *valid_inputs_list[] = {"low", "l", "high", "h", "perfect", "p", "undo", "u", "exit"};

    // save current values in case of undo
    mpfr_t prev_curr, prev_prev_curr;
    mpfr_t prev_low, prev_prev_low;
    mpfr_t prev_high, prev_prev_high;
    mpfr_t tmp_ratio, prev_ratio, prev_prev_ratio;
    mpfr_inits2(BIT_PRECISION, prev_curr, prev_low, prev_high, prev_prev_curr, prev_prev_low, prev_prev_high, tmp_ratio, prev_ratio, prev_prev_ratio, NULL);
    mpfr_set(prev_curr, current_number, MPFR_RNDN);
    mpfr_set(prev_low, low_number, MPFR_RNDN);
    mpfr_set(prev_high, high_number, MPFR_RNDN);
    mpfr_set(prev_prev_curr, current_number, MPFR_RNDN);
    mpfr_set(prev_prev_low, low_number, MPFR_RNDN);
    mpfr_set(prev_prev_high, high_number, MPFR_RNDN);
    mpfr_set(tmp_ratio, ratio, MPFR_RNDN);
    mpfr_set(prev_ratio, ratio, MPFR_RNDN);
    mpfr_set(prev_prev_ratio, ratio, MPFR_RNDN);

    // ask user if number is too low or too high, adjust accordingly
    bool wait = false;
    while (1)
    {
        printHighLowPrompt(&high_low_prompt);
        while (1)
        {

            if (wait && !checkLowValueHotkeyPressed() && !checkHighValueHotkeyPressed() && !checkExitHotkeyPressed() && !checkUndoHotkeyPressed() && !checkRevolutionHotkeyPressed())
            {
                wait = false;
            }
            if (wait)
            {
                continue;
            }

            if (checkRevolutionHotkeyPressed() && !wait)
            {
                wait = true;
                char test_prompt[5000];
                sprintf(test_prompt, "revolution.exe 0.8 0.022 %s 1", mpfr_to_str(&ratio, BIT_PRECISION));
                system(test_prompt);
                Sleep(10);
                continue;
            }
            if (checkLowValueHotkeyPressed() && !wait) // low
            {
                wait = true;

                printf("Too low, adjusting to higher value:\n");
                trickleDownPreviousState(
                    &current_number, &low_number, &high_number, &ratio,
                    &prev_curr, &prev_low, &prev_high, &prev_ratio,
                    &prev_prev_curr, &prev_prev_low, &prev_prev_high, &prev_prev_ratio);
                // low_number = current_number
                mpfr_set(low_number, current_number, MPFR_RNDN);

                break;
            }
            else if (checkHighValueHotkeyPressed() && !wait) // high
            {
                wait = true;

                printf("Too high, adjusting to lower value:\n");
                trickleDownPreviousState(
                    &current_number, &low_number, &high_number, &ratio,
                    &prev_curr, &prev_low, &prev_high, &prev_ratio,
                    &prev_prev_curr, &prev_prev_low, &prev_prev_high, &prev_prev_ratio);

                // high_number = current_number
                mpfr_set(high_number, current_number, MPFR_RNDN);

                break;
            }
            else if (checkExitHotkeyPressed() && !wait) // perfect/exit
            {
                wait = true;
                exitProgram(
                    &base_number, &current_number, &low_number, &high_number, &ratio,
                    &prev_curr, &prev_low, &prev_high, &prev_ratio,
                    &prev_prev_curr, &prev_prev_low, &prev_prev_high, &prev_prev_ratio,
                    &tmp_ratio);
            }
            else if (checkUndoHotkeyPressed() && !wait) // undo
            {
                // v2_hotkeys
                wait = true;

                // set numbers to previous state
                printf("Undo-ing. Reverting to previous numbers:\n");
                printf("Undo-ing only saves last two instances.\n");

                undoCurrentState(
                    &current_number, &low_number, &high_number, &ratio,
                    &prev_curr, &prev_low, &prev_high, &prev_ratio,
                    &prev_prev_curr, &prev_prev_low, &prev_prev_high, &prev_prev_ratio);

                // print the change, reset the loop
                printCurrentNumbers(&low_number, &high_number, &current_number, &ratio);
                copy_to_clipboard(mpfr_to_str(&current_number, BIT_PRECISION));

                memset(input, '\0', BUFFER_SIZE);
                
                printHighLowPrompt(&high_low_prompt);
                Sleep(10);
                continue;
            }
            Sleep(10);
        }

        // adjust the current value according to user's request
        // current_number = (high_number - low_number) / 2 + low_number
        mpfr_sub(current_number, high_number, low_number, MPFR_RNDN);
        mpfr_div_ui(current_number, current_number, 2, MPFR_RNDN);
        mpfr_add(current_number, current_number, low_number, MPFR_RNDN);

        mpfr_div(tmp_ratio, current_number, prev_curr, MPFR_RNDN); // ratio
        mpfr_mul(ratio, ratio, tmp_ratio, MPFR_RNDN);              // new ratio

        resetLowHighLoop(&prev_curr, &current_number, input);

        // TODO TESTING
        mpfr_printf("Your new sensitivity is " COLOR_BLUE "%s" COLOR_RESET " of your starting sens.\n", mpfr_to_str(&ratio, BIT_PRECISION));
        // TODO TESTING

        if (mpfr_eq(current_number, low_number, BIT_PRECISION) && mpfr_eq(current_number, high_number, BIT_PRECISION))
        {
            printf("Your number is already accurate up to ~120 digits.\n");
            break;
        }
    }

    exitProgram(
        &base_number, &current_number, &low_number, &high_number, &ratio,
        &prev_curr, &prev_low, &prev_high, &prev_ratio,
        &prev_prev_curr, &prev_prev_low, &prev_prev_high, &prev_prev_ratio,
        &tmp_ratio);
}

// print high low prompt
void printHighLowPrompt(char **high_low_prompt)
{
    printf(COLOR_BLUE "%s" COLOR_RESET, *high_low_prompt);
}

// after undoing a change, move previous values up to reflect new current state
void trickleDownPreviousState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio,
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio,
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio)
{
    // TODO: print reverting (high or low), undo-ing again reverts (high or low)

    mpfr_set(*prev_prev_curr, *prev_curr, MPFR_RNDN);
    mpfr_set(*prev_prev_low, *prev_low, MPFR_RNDN);
    mpfr_set(*prev_prev_high, *prev_high, MPFR_RNDN);
    mpfr_set(*prev_prev_ratio, *prev_ratio, MPFR_RNDN);
    mpfr_set(*prev_curr, *current_number, MPFR_RNDN);
    mpfr_set(*prev_low, *low_number, MPFR_RNDN);
    mpfr_set(*prev_high, *high_number, MPFR_RNDN);
    mpfr_set(*prev_ratio, *ratio, MPFR_RNDN);
}

void undoCurrentState(
    mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio,
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio,
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio)
{

    mpfr_set(*current_number, *prev_curr, MPFR_RNDN);
    mpfr_set(*low_number, *prev_low, MPFR_RNDN);
    mpfr_set(*high_number, *prev_high, MPFR_RNDN);
    mpfr_set(*ratio, *prev_ratio, MPFR_RNDN);
    mpfr_set(*prev_curr, *prev_prev_curr, MPFR_RNDN);
    mpfr_set(*prev_low, *prev_prev_low, MPFR_RNDN);
    mpfr_set(*prev_high, *prev_prev_high, MPFR_RNDN);
    mpfr_set(*prev_ratio, *prev_prev_ratio, MPFR_RNDN);
}

void resetLowHighLoop(mpfr_t * prev_number, mpfr_t *current_number, char *input)
{
    // reset the loop, part 1
    // print current value
    mpfr_printf(COLOR_MAGENTA "Previous Number: %s\n" COLOR_RESET, mpfr_to_str(prev_number, BIT_PRECISION));
    mpfr_printf(COLOR_GREEN "Current Number: %s\n" COLOR_RESET, mpfr_to_str(current_number, BIT_PRECISION));

    // copy_to_clipboard(mpfr_to_str(current_number, BIT_PRECISION));
    // printf(" (Saved to clipboard.)\n");

    // reset the loop, part 2
    // clear input string
    memset(input, '\0', BUFFER_SIZE);
}

void exitProgram(
    mpfr_t *base_number, mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *ratio,
    mpfr_t *prev_curr, mpfr_t *prev_low, mpfr_t *prev_high, mpfr_t *prev_ratio,
    mpfr_t *prev_prev_curr, mpfr_t *prev_prev_low, mpfr_t *prev_prev_high, mpfr_t *prev_prev_ratio,
    mpfr_t *tmp_ratio)
{
    printCurrentNumbers(low_number, high_number, current_number, ratio);
    copy_to_clipboard(mpfr_to_str(current_number, BIT_PRECISION));
    mpfr_printf(COLOR_GREEN "COPIED TO CLIPBOARD. Put your new sensitivity in and have fun!" COLOR_RESET "\n");
    mpfr_clears(
        *base_number, *current_number, *low_number, *high_number, *ratio,
        *prev_curr, *prev_low, *prev_high, *prev_ratio,
        *prev_prev_curr, *prev_prev_low, *prev_prev_high, *prev_prev_ratio,
        *tmp_ratio, NULL);
    mpfr_free_cache();
    exit(0);
}