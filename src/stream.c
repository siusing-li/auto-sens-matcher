#include "stream.h"
#include "mpfr.h"

bool is_valid_float_string(char *str)
{
    size_t i = 0;
    bool has_digit = false, has_dot = false;

    // Optional leading sign
    if (str[i] == '+' || str[i] == '-')
    {
        i++;
    }

    while (str[i])
    {
        if (isdigit(str[i]))
        {
            has_digit = true;
        }
        else if (str[i] == '.')
        {
            if (has_dot)
                return false;
            has_dot = true;
        }
        else
        {
            return false;
        }
        i++;
    }

    return has_digit;
}

void askFloatUserInput(char **prompt, mpfr_t *dest)
{
    mpfr_t value;
    mpfr_init2(value, BIT_PRECISION);
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf(COLOR_BLUE "%s" COLOR_RESET, *prompt);

        if (fgets(buffer, BUFFER_SIZE, stdin))
        {
            trim_newline(buffer);
            trim_whitespace(buffer);
            if (strcmp(buffer, "exit") == 0)
            {
                exit(0);
            }
            if (is_valid_float_string(buffer))
            {
                mpfr_set_str(value, buffer, BASE_TEN, MPFR_RNDN);
                mpfr_set(*dest, value, MPFR_RNDN);
                return;
            }
        }
        printf("Try again with a valid value. \n");
    }
}

void trim_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

void trim_whitespace(char *str)
{
    char *start = str;
    char *end;

    // Move start to first non-whitespace character
    while (isspace((unsigned char)*start))
    {
        start++;
    }

    if (*start == '\0')
    {
        str[0] = '\0';

        return;
    }

    // Find end of string
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }

    *(end + 1) = '\0';

    // Move trimmed string to beginning
    memmove(str, start, end - start + 2); // +1 for null terminator
}

void askStringUserInput(char **prompt, char *input, char **valid_inputs_list, const size_t list_size)
{
    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    // char *stream = (char *)calloc(BUFFER_SIZE, sizeof(char));
    
    while (1)
    {
        printf(COLOR_BLUE "%s" COLOR_RESET, *prompt);

        // if (sscanf(stream, "%s", buffer) || fgets(buffer, BUFFER_SIZE, stdin))
        if (fgets(buffer, BUFFER_SIZE, stdin))
        {

            trim_newline(buffer);
            trim_whitespace(buffer);

            for (size_t i = 0; buffer[i]; i++)
            {
                buffer[i] = tolower(buffer[i]);
            }

            for (size_t i = 0; i < list_size; i++)
            {
                if (strcmp(buffer, valid_inputs_list[i]) == 0)
                {
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

void askInitialValues(mpfr_t *current_number, mpfr_t *low_number, mpfr_t *high_number, mpfr_t *base_number)
{
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
    while (1)
    {
        askFloatUserInput(&min_number_prompt, low_number);
        if (mpfr_cmp(*low_number, *base_number) <= 0)
        {
            break;
        }
        printf("Less than or equal to your base number.\n");
    }
    mpfr_printf("MIN number set to: %s\n", mpfr_to_str(low_number, BIT_PRECISION));

    // get max number
    char *max_number_prompt = "Please enter a MAX number/decimal value: ";
    while (1)
    {
        askFloatUserInput(&max_number_prompt, high_number);
        if (mpfr_cmp(*high_number, *base_number) >= 0)
        {
            break;
        }
        printf("Greater than or equal to your base number.\n");
    }
    mpfr_printf("MAX number set to: %s\n", mpfr_to_str(high_number, BIT_PRECISION));
}

void copy_to_clipboard(const char *text)
{
    const size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), text, len);
    GlobalUnlock(hMem);

    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

void printCurrentNumbers(mpfr_t *low_number, mpfr_t *high_number, mpfr_t *current_number)
{
    mpfr_printf("Low Number: %s\n", mpfr_to_str(low_number, BIT_PRECISION));
    mpfr_printf("High Number: %s\n", mpfr_to_str(high_number, BIT_PRECISION));
    mpfr_printf(COLOR_GREEN "YOUR NUMBER: %s" COLOR_RESET "\n", mpfr_to_str(current_number, BIT_PRECISION));
}
