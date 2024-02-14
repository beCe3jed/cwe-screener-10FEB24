#include<stdio.h>
#include<stdlib.h>

int main(void) {
    unsigned int var1 = 0x1010101; // this one might actually be a char?
    unsigned int var2 = 0x2020202;
    unsigned int var4 = 0x4040404;
    unsigned int var8 = 0x8080808;
    unsigned int cpy1 = 0x10101010;
    unsigned int cpy2 = 0x20202020;
    unsigned int cpy4 = 0x40404040;
    unsigned int cpy8 = 0x80808080;

    putchar((int)(char)var2 | var4 | cpy2 | cpy4);
    putchar((int)(char)var4 | var8 | cpy2 | cpy4);
    putchar((int)var1 | cpy2 | cpy4);
    putchar((int)var1 | var2 | var4 | cpy2 | cpy4);
    putchar((int)var1 | var2 | var8 | cpy1 | cpy2 | cpy4);

    putchar((int)var1 | var2 | var4 | cpy1 | cpy2 | cpy4);
    putchar((int)(char)var8 | cpy2 | cpy4);
    putchar((int)var1 | cpy2 | cpy4);
    putchar((int)var4 | cpy1 | cpy2 | cpy4);
    putchar((int)var8 | var1 | var2 | var4 | cpy1 | cpy4);
    putchar((int)var1 | cpy2 | cpy4);
    putchar((int)var1 | var2 | var4 | var8 | cpy1 | cpy4);
    putchar((int)var1 | var4 | var8 | cpy2 | cpy4);
    putchar((int)var1 | var4 | cpy2 | cpy4 );
    putchar((int)var1 | var2 | cpy1 | cpy2 | cpy4);
    putchar((int)var1 | var2 | cpy1 | cpy2 | cpy4);





    return 0;
}