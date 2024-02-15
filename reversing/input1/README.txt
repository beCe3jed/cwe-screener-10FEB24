This problem is very straightforward. We give an input and it gets manipulated then finally checked against a key. If it equals the key, we get the flag.

Lets look at main():

--------------------------------------------------------------------------------------------------------------------------------------------------------
undefined8 main(void)
{
  long in_FS_OFFSET;
  undefined4 local_18;
  int local_14;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  setvbuf(stdout,(char *)0x0,2,0);
  setvbuf(stderr,(char *)0x0,2,0);
  puts("Do you know the correct number to input?");
  __isoc99_scanf(&DAT_00102031,&local_18);
  local_14 = check(local_18);
  if (local_14 != 0xb88202) {
    puts("You lose!");
                    /* WARNING: Subroutine does not return */
    exit(0);
  }
  puts("Congratulations!");
  system("cat flag.txt");
  if (local_10 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return 0;
}
--------------------------------------------------------------------------------------------------------------------------------------------------------


and now check():
--------------------------------------------------------------------------------------------------------------------------------------------------------
uint check(uint param_1)
{
  return (param_1 ^ 0x735) >> 4 ^ 0x6f0;
}
--------------------------------------------------------------------------------------------------------------------------------------------------------

so since we have the formula, we can easily reverse it.


and now exploit.py
--------------------------------------------------------------------------------------------------------------------------------------------------------
final = 0xb88202
print(hex(final))

tier1 = final ^ 0x6f0 	# 0xb88442
print(hex(tier1))

tier2 = tier1 << 4 		# 0xb884420
print(hex(tier2))

myInput = tier2 ^ 0x735	# 0xb884315
print("Your input should be: " + str(myInput))
--------------------------------------------------------------------------------------------------------------------------------------------------------
