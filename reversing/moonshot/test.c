#include<stdio.h>

void print_mystery(char var0x1010101,unsigned int var0x2020202,unsigned int var0x4040404,unsigned int var0x8080808,unsigned int cpy0x10101010,unsigned int cpy0x20202020,unsigned int cpy0x40404040);

int main(void){
    unsigned int var0x10101 = 0x1010101;
    unsigned int var0x20202 = 0x2020202;
    unsigned int var0x4040 = 0x4040404;
    unsigned int var0x808080 = 0x8080808;
    unsigned int cpy0x10101 = 0x10101010;
    unsigned int cpy0x20202 = 0x20202020;
    unsigned int cpy0x404040 = 0x40404040;
    unsigned int cpy0x808080 = 0x80808080;
    
    print_mystery(var0x10101,var0x20202,var0x4040,var0x808080,cpy0x10101,cpy0x20202,cpy0x404040); // cpy0x808080 removed

    return 0;
}

void print_mystery(char var0x1010101,unsigned int var0x2020202,unsigned int var0x4040404,unsigned int var0x8080808,unsigned int cpy0x10101010,unsigned int cpy0x20202020,unsigned int cpy0x40404040)
{
    putchar((int)(char)var0x2020202 | var0x4040404 | cpy0x20202020 | cpy0x40404040);
    putchar((int)(char)var0x4040404 | var0x8080808 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | var0x4040404 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | var0x8080808 | cpy0x10101010 | cpy0x20202020 |
            cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | var0x4040404 | cpy0x10101010 | cpy0x20202020 |
            cpy0x40404040);
    putchar((int)(char)var0x8080808 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | cpy0x20202020 | cpy0x40404040);
    putchar((int)(char)var0x4040404 | cpy0x10101010 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | var0x4040404 | var0x8080808 | cpy0x10101010 |
            cpy0x40404040);
    putchar((int)var0x1010101 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | var0x4040404 | var0x8080808 | cpy0x10101010 |
            cpy0x40404040);
    putchar((int)var0x1010101 | var0x4040404 | var0x8080808 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x4040404 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | cpy0x10101010 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x2020202 | cpy0x10101010 | cpy0x20202020 | cpy0x40404040);
    putchar((int)var0x1010101 | var0x4040404 | var0x8080808 | cpy0x10101010 | cpy0x20202020 |
            cpy0x40404040);
    putchar((int)(char)var0x2020202 | var0x8080808);
  return;
}
