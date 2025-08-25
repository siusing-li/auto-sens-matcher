#include "revolution.h"

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
    // 0.535    0.022   1
    // 0.328    0.022   5
    // 0.8      0.022   0
    long double sensitivity = 0.8;
    long double yaw = 0.022;

    long double revolution = 360.0;
    long double increment = sensitivity * yaw;
    long double total_counts = revolution / increment;

    printf("sens: %.3Lf | yaw: %.3Lf\n", sensitivity, yaw);
    printf("increment: %.6Lf deg/count\n", increment);
    printf("total counts for 360 deg: %.10Lf\n", total_counts);

    double total_counts_int;
    long double total_counts_fractional;
    total_counts_fractional = modf(total_counts, &total_counts_int);

    int step = (int)(total_counts_int); // pixels per input

    int threshold[] = {400, 600};
    int inaccuracy = 0;
    while (1)
    {
        // printf("old step %d\n", step);
        for (int i = threshold[1]; i >= threshold[0]; i--)
        {
            if (step % i == 0)
            {
                step = i;
                // printf("new step %d\n", step);
                break;
            }
        }
        if (step >= threshold[0] && step <= threshold[1])
        {
            break;
        }
        step = (int)(step + 1);
        inaccuracy += 1;
    }

    printf("step %d\n", step);
    printf("inaccuracy by %d\n", inaccuracy);
    unsigned long steps_needed = (total_counts_int + inaccuracy) / step;
    printf("round steps needed %lf\n", total_counts_int);
    printf("i steps needed %lu\n", steps_needed);
    printf("fractional steps needed %6Lf\n", total_counts_fractional);

    int j = 0;
    long double fractional_sum = 0;
    while (1)
    {
        if (checkHotkeyPressed())
        {
            printf("Hotkey pressed! Simulating revolution...\n");

            // perform mouse revolution 
            for (int i = 0; i < steps_needed; i++)
            {
                moveMouseRelative(step, 0);
                Sleep(1);
            }

            // fix inaccuracy, perform (-inaccuracy) mouse adjustment
            if (inaccuracy > 0)
            {
                printf("inaccuracy adjustment %d\n", -inaccuracy);
                moveMouseRelative(-inaccuracy, 0);
            }

            // perform fractional mouse movement
            // mouse movement function does not allow moving (decimal) amount of pixels
            // at n revolutions, if fractional
            fractional_sum += total_counts_fractional;
            double sum_floor = floor(fractional_sum);

            printf("sum %6Lf, floor %lf, k %d\n", fractional_sum, sum_floor, j);
            if ((int)sum_floor > j)
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