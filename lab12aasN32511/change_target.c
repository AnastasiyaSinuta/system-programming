#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* convertToDecimal(const char* number) {
    int base = 0;
    int i = 0;
    int decimal = 0;
    
    if (number[0] == '0' && number[1] == 'b') {
        base = 2;
        i = 2;
    } else if (number[0] == '0' && (number[1] == 'x' || number[1] == 'X')) {
        base = 16;
        i = 2;
    } else {
        return (char*) number;
    }
    
    int len = strlen(number);
    for (; i < len; i++) {
        char digit = number[i];
        int value;
        
        if (digit >= '0' && digit <= '9') {
            value = digit - '0';
        } else if (digit >= 'A' && digit <= 'F') {
            value = 10 + (digit - 'A');
        } else if (digit >= 'a' && digit <= 'f') {
            value = 10 + (digit - 'a');
        } else {
            char* error = "Invalid number format. Non-numeric digit found.";
            return error;
        }

        decimal += value * pow(base, len - i - 1);
    }
    char buffer[20];
    sprintf(buffer, "%d", decimal);
    char* result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}
