#include "mpfr.h"

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
