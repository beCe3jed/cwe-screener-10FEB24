Let's get started with moonshot. After runnning it, it looks like that it requests a password, and immediately exits upon incorrect input.
------------------------------------------------------------------------------------------------------------------------------------------------
┌──(kali㉿kali)-[~/Hacking/cwe_screener_2.0/reversing/moonshot]
└─$ ./moonshot 
I'm thinking of a number between negative infinity and infinity...
12345768
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_2.0/reversing/moonshot]
└─$ 
------------------------------------------------------------------------------------------------------------------------------------------------
There's a lot more testing that we could do, but let's jump to opening up this file in ghidra...

We see that main has a lot going on:
------------------------------------------------------------------------------------------------------------------------------------------------
undefined8 main(uint param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint buffer;
  uint cpy0x808080;
  uint cpy0x404040;
  uint cpy0x20202;
  uint cpy0x10101;
  uint var0x808080;
  uint var0x4040;
  uint var0x20202;
  uint var0x10101;
  uint argc?;
  int iterator7;
  int iterator6;
  int iterator5;
  int iterator4;
  int iterator3;
  int iterator2;
  int iterator1;
  int local;
  
  buffer = 0;
  var0x10101 = 0x1010101;
  var0x20202 = 0x2020202;
  var0x4040 = 0x4040404;
  var0x808080 = 0x8080808;
  cpy0x10101 = 0x10101010;
  cpy0x20202 = 0x20202020;
  cpy0x404040 = 0x40404040;
  cpy0x808080 = 0x80808080;
  local = 0;
  argc? = param_1;
  do {
    if (1 < local) {
      return 0;
    }
    for (iterator1 = 0; iterator1 < 2; iterator1 = iterator1 + 1) {
      for (iterator2 = 0; iterator2 < 2; iterator2 = iterator2 + 1) {
        for (iterator3 = 0; iterator3 < 2; iterator3 = iterator3 + 1) {
          for (iterator4 = 0; iterator4 < 2; iterator4 = iterator4 + 1) {
            for (iterator5 = 0; iterator5 < 2; iterator5 = iterator5 + 1) {
              for (iterator6 = 0; iterator6 < 2; iterator6 = iterator6 + 1) {
                for (iterator7 = 0; iterator7 < 2; iterator7 = iterator7 + 1) {
                  puts("I\'m thinking of a number between negative infinity and infinity...");
                  __isoc99_scanf(&format,&buffer);
                  if (argc? == (var0x10101 & var0x20202 & var0x4040 & var0x808080 & cpy0x10101 &
                                cpy0x20202 & cpy0x404040 & cpy0x808080)) {
print_flag_beg:
                    printf("flag{");
red_Herring:
                    printf("r3d_h3rr1ng");
print_flag_end:
                    putchar(0x7d);
                    return 0;
                  }
                  if (var0x10101 == var0x20202) goto print_flag_end;
                  if (var0x20202 == var0x4040) goto print_flag_beg;
                  if ((buffer & argc?) != 0) {
                    /* buffer should be equal to one of the uVars */
                    uVar1 = var0x10101;
                    if (local == 0) {
                      uVar1 = 0;
                    }
                    uVar2 = var0x20202;
                    if (iterator1 == 0) {
                      uVar2 = 0;
                    }
                    uVar3 = var0x4040;
                    if (iterator2 == 0) {
                      uVar3 = 0;
                    }
                    uVar4 = var0x808080;
                    if (iterator3 == 0) {
                      uVar4 = 0;
                    }
                    uVar5 = cpy0x10101;
                    if (iterator4 == 0) {
                      uVar5 = 0;
                    }
                    uVar6 = cpy0x20202;
                    if (iterator5 == 0) {
                      uVar6 = 0;
                    }
                    uVar7 = cpy0x404040;
                    if (iterator6 == 0) {
                      uVar7 = 0;
                    }
                    uVar8 = cpy0x808080;
                    if (iterator7 == 0) {
                      uVar8 = 0;
                    }
                    if ((buffer | argc?) ==
                        (uVar8 | uVar1 | uVar2 | uVar3 | uVar4 | uVar5 | uVar6 | uVar7)) {
                      print_mystery(var0x10101,var0x20202,var0x4040,var0x808080,cpy0x10101,              <---------------------- PRINT_MYSTERY
                                    cpy0x20202,cpy0x404040,cpy0x808080);
                    }
                  }
                  if (var0x4040 == var0x808080) goto red_Herring;
                  buffer = buffer + 1;
                    /* buffer must be below 2 */
                  if (3 < (int)buffer) {
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    }
    local = local + 1;
  } while( true );
}
------------------------------------------------------------------------------------------------------------------------------------------------
There are clearly a lot of checks and hurdles to jump through to get the password... there's even a red herring that tries to trick us... but
why are we going through all these hurdles...? what happens assuming we get pass through all of them? is there some type of function that executes?

after taking a closer look... we see that there is a function "print_mystery".... lets check out the "print_mystery" function...
------------------------------------------------------------------------------------------------------------------------------------------------
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
------------------------------------------------------------------------------------------------------------------------------------------------
Well this looks an awful like a function to print the flag to the screen, we just need to know what all these variables represent. Thankfully, 
we can look back in main at the function call and the variable declarations. We can try to type all this out, or... we can just copy and paste
this data into our own program: test.c
------------------------------------------------------------------------------------------------------------------------------------------------
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
------------------------------------------------------------------------------------------------------------------------------------------------
upon compilation, we see that there is one extra parameter that is passed that doesn't match the function definition. So after some investigating, 
we see that 8 params are passed, but only 7 are defined in the definition, and the last parameter that was passed "cpy0x808080" doesn't look like 
it gets used in the definition so i removed that from the function call.

After running the program this is what we get: (note stdio.h was imported to get rid of the warnings at compilation)
------------------------------------------------------------------------------------------------------------------------------------------------
┌──(kali㉿kali)-[~/Hacking/cwe_screener_2.0/reversing/moonshot]
└─$ ./test                                                                                                                                                                       
flag{what_a_mess}
------------------------------------------------------------------------------------------------------------------------------------------------
Looks like it was pretty easy after all!


