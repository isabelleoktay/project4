#include<stdio.h>

void decToHex(int dec);
char intToHex[4];


int main()
{
    decToHex(59);

    return 0;
}

void decToHex(int dec) {

    int quot, rem, i, j;
    j = 0;

    quot = dec;
    while (quot != 0) {
        rem = quot % 16;
        if (rem < 10) {
            intToHex[j] = 48 + rem;
            j++;
        }
        else {
            intToHex[j] = 55 + rem;
            j++;
        }
        quot /= 16;
    }

    printf("0x");
    for (i = j; i >= 0; i--) {
        printf("%c", intToHex[i]);
    }
    printf("\n");

    return;

}

