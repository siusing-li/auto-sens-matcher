#include "revolution.h"
#include "mpfr.h"
#include "stream.h"

void moveMouseRelative(int dx, int dy)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE; // relative movement
    input.mi.dx = dx;
    input.mi.dy = dy;
    SendInput(1, &input, sizeof(INPUT));
}

/* // SetCurPos method, BETA WIP
 void moveMouseRelative(int x, int y) {
     POINT currentPos;
     GetCursorPos(&currentPos);
     SetCursorPos(currentPos.x + x, currentPos.y + y);
 }*/

int checkHotkeyPressed()
{
    // Check if Alt key (VK_MENU) and Insert (VK_INSERT) are pressed
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_INSERT) & 0x8000);
}

int main()
{
    // inputs
    mpfr_t sensitivity;
    mpfr_t yaw;

    // calculations
    mpfr_t revolution;
    mpfr_t increment;
    mpfr_t total_counts;
    // init all variables
    mpfr_inits2(BIT_PRECISION, sensitivity, yaw, revolution, increment, total_counts, NULL);
    
    // 0.535    0.022   1
    // 0.328    0.022   5
    // 0.8      0.022   0
    mpfr_set_str(sensitivity, "0.8", 10, MPFR_RNDN); // sensitivity = 0.8
    mpfr_set_str(yaw, "0.022", 10, MPFR_RNDN); // yaw = 0.022
    mpfr_set_str(revolution, "360", 10, MPFR_RNDN); // revolution = 360.0
    
    // increment = sensitivity * yaw;
    mpfr_mul(increment, sensitivity, yaw, MPFR_RNDN);
    // total_counts = revolution / increment;
    mpfr_div(total_counts, revolution, increment, MPFR_RNDN);
    

    printf("Sens: %s | Yaw: %s\n", mpfr_to_str(&sensitivity, BIT_PRECISION), mpfr_to_str(&yaw, BIT_PRECISION));
    printf("Increment: %s deg/count\n", mpfr_to_str(&increment, BIT_PRECISION));
    printf("Total counts for 360 deg: %s\n", mpfr_to_str(&total_counts, BIT_PRECISION));
    printf("Press Alt + Insert to do a 360.\n");

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
            if (mpfr_get_ui(remainder, MPFR_RNDN) == 0) // if (step % i == 0)
            {
                mpfr_set_ui(step, i, MPFR_RNDN);// step = i;
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
    // printf("step %s\n", mpfr_to_str(&step, BIT_PRECISION));
    // printf("inaccuracy by %ld\n", inaccuracy);

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

    unsigned long j = 0; // number of revolutions
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
    while (1)
    {
        // 1. wait for hotkey
        if (checkHotkeyPressed())
        {
            printf("Hotkey pressed! Simulating revolution...\n");

            // 2. move mouse to do revolution (with inaccuracy)
            // for (int i = 0; i < steps_needed; i++)
            for (unsigned long i = 0; i < mpfr_get_ui(steps_needed, MPFR_RNDN) ; i++)
            {
                // moveMouseRelative(step, 0);
                unsigned long moveInt = mpfr_get_ui(step, MPFR_RNDN);
                printf("%ld under %ld, moving %s, %ld\n", i, mpfr_get_ui(steps_needed, MPFR_RNDN), mpfr_to_str(&step, BIT_PRECISION), moveInt);
                moveMouseRelative(moveInt, 0);
                Sleep(1);
            }

            // 3. mouse mouse to fix inaccuracy
            // perform (-inaccuracy) mouse adjustment
            if (inaccuracy > 0)
            {
                printf("inaccuracy adjustment %ld\n", -inaccuracy);
                moveMouseRelative(-inaccuracy, 0);
            }

            // 4. perform fractional mouse movement
            // mouse movement function does not allow moving (decimal) amount of pixels
            // at j revolutions, if fractional sum reaches n*1, then move 1 pixel
            // 
            // fractional_sum += total_counts_fractional;
            mpfr_add(fractional_sum, fractional_sum, total_counts_fractional, MPFR_RNDN);
            // double fractional_sum_floor = floor(fractional_sum);
            mpfr_floor(fractional_sum_floor, fractional_sum);

            printf("sum %s, floor %s, k %ld\n", mpfr_to_str(&fractional_sum, BIT_PRECISION), mpfr_to_str(&fractional_sum_floor, BIT_PRECISION), j);
            // if ((int)fractional_sum_floor > j)
            if (mpfr_get_ui(fractional_sum_floor, MPFR_RNDN) > j)
            {
                j++;
                printf("fractional adjustment\n");
                moveMouseRelative(1, 0);
            }
        }

        Sleep(30); // reduce cpu usage
    }
    printf("Done.\n");
    return 0;
}