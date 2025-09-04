#include "revolution.h"
#include "mpfr.h"
#include "stream.h"

int main(int argc, char **argv)
{
    checkProgramParameters(&argc, &argv, 4 + 1);
    // checkProgramParameters(&argc, &argv, 5 + 1);

    // inputs
    mpfr_t sensitivity;                                         // argv[1]
    mpfr_t yaw;                                                 // argv[2]
    mpfr_t ratio;                                               // argv[3]
    bool force = strcmp(argv[argc - 1], "1") == 0;              // argv[4]
    // long long revolution_threshold_max = str_to_ulong(argv[5]); // argv[5]
    

    // calculations
    mpfr_t revolution;
    mpfr_t increment;
    mpfr_t total_counts;

    // init all variables
    mpfr_inits2(BIT_PRECISION, sensitivity, yaw, revolution, increment, total_counts, ratio, NULL);

    // 0.535    0.022   1
    // 0.328    0.022   5
    // 0.8      0.022   0
    mpfr_set_str(sensitivity, argv[1], 10, MPFR_RNDN); // sensitivity = 0.8
    mpfr_set_str(yaw, argv[2], 10, MPFR_RNDN);         // yaw = 0.022
    mpfr_set_str(ratio, argv[3], 10, MPFR_RNDN);       // revolution = 360.0
    mpfr_set_str(revolution, "360", 10, MPFR_RNDN);    // revolution = 360.0

    // increment = sensitivity * yaw;
    mpfr_mul(increment, sensitivity, yaw, MPFR_RNDN);
    // total_counts = revolution / increment;
    mpfr_div(total_counts, revolution, increment, MPFR_RNDN);

    // printf("REFERENCE Total counts for 360 deg: %s\n", mpfr_to_str(&total_counts, BIT_PRECISION));
    mpfr_mul(total_counts, total_counts, ratio, MPFR_RNDN); // new rev
    // printf("TESTING Total counts for 360 deg: %s\n", mpfr_to_str(&total_counts, BIT_PRECISION));

    printf("Using these numbers as reference with ratio of %s\n", mpfr_to_str(&ratio, BIT_PRECISION));
    printf("Sens: %s | Yaw: %s\n", mpfr_to_str(&sensitivity, BIT_PRECISION), mpfr_to_str(&yaw, BIT_PRECISION));
    printf("Increment: %s deg/count\n", mpfr_to_str(&increment, BIT_PRECISION));
    // printf("Total counts for 360 deg: %s\n", mpfr_to_str(&total_counts, BIT_PRECISION));

    // split "total count" to an int and a fractional
    mpfr_t total_counts_int;
    mpfr_t total_counts_fractional;
    mpfr_inits2(BIT_PRECISION, total_counts_int, total_counts_fractional, NULL);
    mpfr_modf(total_counts_int, total_counts_fractional, total_counts, MPFR_RNDN);

    // initialize a variable to move certain # pixels every step
    // (instead of moving 1 pixel at a time, very slow, laggy)
    mpfr_t step;
    mpfr_init2(step, BIT_PRECISION);
    mpfr_set(step, total_counts_int, MPFR_RNDN);

    // choose a range between threshold[0] and threshold[1] pixels
    // i.e. how many moves per step
    unsigned long threshold[] = {400, 600};
    unsigned long inaccuracy = 0;
    while (1)
    {
        mpfr_t remainder; // pixels per input
        mpfr_init2(remainder, BIT_PRECISION);
        for (unsigned long i = threshold[1]; i >= threshold[0]; i--)
        {
            mpfr_fmod_ui(remainder, step, i, MPFR_RNDN); // get remainder of step / i
            if (mpfr_get_ui(remainder, MPFR_RNDN) == 0)  // check remainder (`step % i`) == 0
            {
                mpfr_set_ui(step, i, MPFR_RNDN); // step = i;
                break;
            }
        }
        mpfr_clear(remainder);

        // if (step >= threshold[0] && step <= threshold[1])
        if (mpfr_cmp_ui(step, threshold[0]) >= 0 && mpfr_cmp_ui(step, threshold[1]) <= 0)
        {
            break;
        }

        mpfr_add_ui(step, step, 1, MPFR_RNDN); // step = (int)(step + 1);
        inaccuracy += 1;
    }
    // printf("Step %s\n", mpfr_to_str(&step, BIT_PRECISION));

    if (!force) 
    {
        printf("Press Alt + Insert to do a 360.\n");
    }

    // calculate how many steps needed to do 360
    // steps_needed = (total_counts_int + inaccuracy) / step;
    mpfr_t steps_needed;
    mpfr_init2(steps_needed, BIT_PRECISION);
    mpfr_add_ui(steps_needed, total_counts_int, inaccuracy, MPFR_RNDN);
    mpfr_div(steps_needed, steps_needed, step, MPFR_RNDN);
    mpfr_floor(steps_needed, steps_needed);

    // printf("round pixels needed %s\n", mpfr_to_str(&total_counts_int, BIT_PRECISION));
    // printf("i steps needed %s\n", mpfr_to_str(&steps_needed, BIT_PRECISION));
    // printf("fractional steps needed %s\n", mpfr_to_str(&total_counts_fractional, BIT_PRECISION));

    unsigned long j = 0;   // iterator: number of revolutions
    mpfr_t fractional_sum; // sum of fractional movements
    mpfr_init2(fractional_sum, BIT_PRECISION);
    mpfr_set_d(fractional_sum, 0.0, MPFR_RNDN); // fractional_sum = 0;
    mpfr_t fractional_sum_floor;
    mpfr_init2(fractional_sum_floor, BIT_PRECISION);

    // move mouse to do revolution
    // 1. wait for hotkey
    // 2. move mouse to do revolution (with inaccuracy)
    // 3. mouse mouse to fix inaccuracy
    // 4. add up fractional movements until reaches n*1,
    //     4a. if so, move 1 pixel.
    unsigned long revolution_threshold[] = {20, 30};
    while (1)
    {
        int max_revolutions = 1;
        bool multi_pressed = false;
        // 1. wait for hotkey
        if (checkMultiRevolutionHotkeyPressed())
        {
            max_revolutions = revolution_threshold[1];
            // max_revolutions = NUM_REVOLUTIONs_ON_MULTI_PRESS;
            multi_pressed = true;
        }
        if (force || checkRevolutionHotkeyPressed() || multi_pressed)
        {
            printf("Simulating revolution...\n");

            int i = 0;
            while (i < max_revolutions)
            // for (int i = 0; i < max_revolutions; i++)
            {
                // 2. move mouse to do revolution (with inaccuracy)
                // for (int i = 0; i < steps_needed; i++)
                for (unsigned long i = 0; i < mpfr_get_ui(steps_needed, MPFR_RNDN); i++)
                {
                    // moveMouseRelative(step, 0);
                    unsigned long moveInt = mpfr_get_ui(step, MPFR_RNDN);
                    moveMouseRelative(moveInt, 0);
                    Sleep(1);
                }

                // 3. mouse mouse to fix inaccuracy
                // perform (-inaccuracy) mouse adjustment
                if (inaccuracy > 0)
                {
                    // printf("[%d.] inaccuracy adjustment %ld\n", i + 1, -inaccuracy);
                    moveMouseRelative(-inaccuracy, 0);
                    Sleep(1);
                }

                // 4. perform fractional mouse movement
                // mouse movement function does not allow moving (decimal) amount of pixels
                // at j revolutions, if fractional sum reaches n*1, then move 1 pixel
                //
                // fractional_sum += total_counts_fractional;
                mpfr_add(fractional_sum, fractional_sum, total_counts_fractional, MPFR_RNDN);
                // double fractional_sum_floor = floor(fractional_sum);
                mpfr_floor(fractional_sum_floor, fractional_sum);

                // printf("sum %s, floor %s, k %ld\n", mpfr_to_str(&fractional_sum, BIT_PRECISION), mpfr_to_str(&fractional_sum_floor, BIT_PRECISION), j);
                // if ((int)fractional_sum_floor > j)
                if (mpfr_get_ui(fractional_sum_floor, MPFR_RNDN) > j)
                {
                    j++;
                    printf("[%d.] fractional adjustment\n", i + 1);
                    moveMouseRelative(1, 0);
                    Sleep(1);
                    if (i > revolution_threshold[0])
                    {
                        max_revolutions = i;
                    }
                }
                printf("[%d.] Success!\n", i + 1);
                i++;
            }
        }

        Sleep(30); // reduce cpu usage
        // only run one revolution and quit
        if (force) {
            break;
        }
    }
    exitRevolution(&sensitivity, &yaw, &ratio, &revolution, &increment, &total_counts, &total_counts_int, &total_counts_fractional, &step, &steps_needed, &fractional_sum, &fractional_sum_floor);
    return 0;
}

void exitRevolution(mpfr_t *sensitivity, mpfr_t *yaw, mpfr_t *ratio, mpfr_t *revolution, mpfr_t *increment, mpfr_t *total_counts, mpfr_t *total_counts_int, mpfr_t *total_counts_fractional, mpfr_t *step, mpfr_t *steps_needed, mpfr_t *fractional_sum, mpfr_t *fractional_sum_floor)
{
    mpfr_clears(*sensitivity, *yaw, *ratio, *revolution, *increment, *total_counts, *total_counts_int, *total_counts_fractional, *step, *steps_needed, *fractional_sum, *fractional_sum_floor, NULL);
    mpfr_free_cache();
}

void moveMouseRelative(int dx, int dy)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE; // relative movement
    input.mi.dx = dx;
    input.mi.dy = dy;
    SendInput(1, &input, sizeof(INPUT));
}

void checkProgramParameters(int *argc, char ***argv, int numArgs)
{
    int arg_pass = parseArgs(argc, argv, numArgs);
    if (arg_pass == 0)
    {
        return;
    }
    printf(COLOR_RED "Invalid Arguments in program. Try again in this format:\n" COLOR_RESET);
    if (arg_pass == -1)
    {
        printf("-1. Invalid number of arguments, expected %d, got %d\n", numArgs - 1, *argc - 1);
    }
    printf("-2. Invalid argument values\n");

    printf(COLOR_GREEN "./revolution.exe (preferred sensitivity in app1) (yaw value of app1) (percentage of your sensitivity) (force?)\n" COLOR_RESET);
    printf(COLOR_BLUE "1. (preferred sensitivity in app1). accepted inputs: [0 - n] - your sensitivity from app1 - ex: 0.8\n" COLOR_RESET);
    printf(COLOR_MAGENTA "2. (yaw value of app1). accepted inputs: [0 - n] - the yaw value of that app1 uses - ex: 0.022\n" COLOR_RESET);
    printf(COLOR_BLUE "3. (percentage of your sensitivity). accepted inputs: [0 - 1] - always \"1\" unless you want this program to perform only a percentage of your sensitivity, input something between 0 and 1 - ex: 0.5 (half of my sensitivity)\n" COLOR_RESET);
    printf(COLOR_MAGENTA "4. (force?). accepted inputs: [0, 1] - (1 = force a revolution without waiting for hotkey, 0 = wait for hotkey to perform revolution)\n" COLOR_RESET);
    exit(0);
}

// check
int parseArgs(int *argc, char ***argv, int numArgs)
{
    if (numArgs == *argc)
    {
        for (int i = 1; i < *argc; i++)
        {
            if (!is_valid_float_string((*argv)[i]))
            {
                return -2;
            }
        }
        if (strcmp((*argv)[numArgs - 1], "1") != 0 && strcmp((*argv)[numArgs - 1], "0") != 0)
        {
            //
            printf("%d, not 1 or 0\n", (*argv)[4]);
            //
            return -2;
        }
        // if (str_to_ulong(argv[5]) <= 0)
        // {
        //     return -2;
        // }

        return 0;
    }
    return -1;
}
