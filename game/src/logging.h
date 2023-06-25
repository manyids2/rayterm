#include "raylib.h"
#include <stdio.h> // Required for: fopen(), fclose(), fputc(), fwrite(), printf(), fprintf(), funopen()
#include <time.h>  // Required for: time_t, tm, time(), localtime(), strftime()

// Custom logging function
void CustomLog(int msgType, const char *text, va_list args);
