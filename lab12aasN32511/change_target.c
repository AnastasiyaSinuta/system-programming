#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
        fprintf(stderr, "The target in decimal notation: %s\n", (char*)number);
        return (char*)number;
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
            fprintf(stderr, "Invalid number format. Non-numeric digit found.\n");
            exit(EXIT_FAILURE);
        }

        decimal += value * pow(base, len - i - 1);
    }
    int len1 = 0;
    int tmp = decimal;
    while(tmp) {
        len1++;
        tmp /= 10;
    }
    char* decimal_str = (char*)malloc((len1 + 1) * sizeof(char));
    sprintf(decimal_str, "%d", decimal);
    fprintf(stderr, "The target in decimal notation: %s\n", decimal_str);
    return decimal_str;
}
