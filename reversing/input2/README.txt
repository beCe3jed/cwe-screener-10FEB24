First we run the program and see that is requests a password. There isnt much else useful from runnning it, so lets disassemble it.

This is what we see in ghidra:
-------------------------------------------------------------------------------------------------------------------------------------
undefined8 main(void)
{
    int32_t iVar1;
    int32_t iVar2;
    undefined8 uVar3;
    int64_t in_FS_OFFSET;
    uint64_t var_30h;
    void *buf;
    int64_t canary;
    
    canary = *(int64_t *)(in_FS_OFFSET + 0x28);
    setvbuf(_stdout, 0, 2, 0);
    setvbuf(_stderr, 0, 2, 0);
    puts("Please enter the password: ");
    read(0, &buf, 0x13);
    iVar1 = check1(&buf);
    iVar2 = check2(&buf);
    if ((iVar1 == 0) || (iVar2 == 0)) {
        puts("You lose!");
        exit(0);
    }
    puts("Congratulations!");
    system("cat flag.txt");
    uVar3 = 0;
    if (canary != *(int64_t *)(in_FS_OFFSET + 0x28)) {
        uVar3 = __stack_chk_fail();
    }
    return uVar3;
}
-------------------------------------------------------------------------------------------------------------------------------------
The main function is pretty straightforward, we read our input into a buffer and then run two checks on the buffer in which we will 
exit if either check returns anything other than 0. If we pass the checks, we will open the flag.

let's look at the first check:
-------------------------------------------------------------------------------------------------------------------------------------
bool check16bytes(long input)

{
  int num_bytes;
  
  for (num_bytes = 0; *(char *)(input + num_bytes) != '\0'; num_bytes = num_bytes + 1) {
  }
  return num_bytes == 0xf;
}
-------------------------------------------------------------------------------------------------------------------------------------
this is a very simple function especially after its been deobfuscated. It simply checks if the string is 16 bytes long and ends with 
a null byte.

Lets inspect the second check:
-------------------------------------------------------------------------------------------------------------------------------------
undefined8 check2(long input)

{
  int i;
  
  i = 0;
  while( true ) {
    if (0xe < i) {
      return 1;
    }
                    /* Checks our input against an internal string but in a convoluted fashion */
    if ((buffers[(long)(i % 3) * 0xf + (long)i] ^ *(byte *)(input + i)) != final[0xe - i]) break;
    i = i + 1;
  }
  return 0;
}
-------------------------------------------------------------------------------------------------------------------------------------
This function is a simple hashing function in which there are two hard-coded strings in memory "buffers[45]" and "final[15]", and we manipulate our input in ways to 
confirm if our password is correct. This would take a lot of time to decipher... but we already know the formula, we just need to know
the strings in order to recreate this algorithm. Well, both strings are hard-coded in memory therefore we can easily look them up either
in gdb or in ghidra. 

Here's a snippet of both arrays in gdb:
-------------------------------------------------------------------------------------------------------------------------------------
 ...
  0x0000555555555265 <+67>:    mov    rax,rdx
   0x0000555555555268 <+70>:    shl    rax,0x4
   0x000055555555526c <+74>:    sub    rax,rdx
   0x000055555555526f <+77>:    lea    rdx,[rax+rcx*1]
   0x0000555555555273 <+81>:    lea    rax,[rip+0x2da6]        # 0x555555558020 <buffers>
   0x000055555555527a <+88>:    add    rax,rdx
   0x000055555555527d <+91>:    movzx  ecx,BYTE PTR [rax]
   0x0000555555555280 <+94>:    mov    eax,DWORD PTR [rbp-0x4]
   0x0000555555555283 <+97>:    movsxd rdx,eax
   0x0000555555555286 <+100>:   mov    rax,QWORD PTR [rbp-0x18]
   0x000055555555528a <+104>:   add    rax,rdx
   0x000055555555528d <+107>:   movzx  eax,BYTE PTR [rax]
   0x0000555555555290 <+110>:   xor    ecx,eax
   0x0000555555555292 <+112>:   mov    eax,0xe
   0x0000555555555297 <+117>:   sub    eax,DWORD PTR [rbp-0x4]
   0x000055555555529a <+120>:   cdqe
   0x000055555555529c <+122>:   lea    rdx,[rip+0x2dad]        # 0x555555558050 <final>
   0x00005555555552a3 <+129>:   movzx  eax,BYTE PTR [rax+rdx*1]
   0x00005555555552a7 <+133>:   cmp    cl,al
   0x00005555555552a9 <+135>:   je     0x5555555552b2 <check2+144>
   0x00005555555552ab <+137>:   mov    eax,0x0
   0x00005555555552b0 <+142>:   jmp    0x5555555552c5 <check2+163>
   0x00005555555552b2 <+144>:   add    DWORD PTR [rbp-0x4],0x1
   0x00005555555552b6 <+148>:   cmp    DWORD PTR [rbp-0x4],0xe
   0x00005555555552ba <+152>:   jle    0x555555555237 <check2+21>
   0x00005555555552c0 <+158>:   mov    eax,0x1
   0x00005555555552c5 <+163>:   pop    rbp
   0x00005555555552c6 <+164>:   ret
gef➤  x/64xb $rip+0x2da6
0x555555558019: 0x00    0x00    0x00    0x00    0x00    0x00    0x00    0xa8                      <================================= THIS IS THE START OF BUFFERS
0x555555558021 <buffers+1>:     0x66    0xe5    0xa2    0xaf    0x8d    0x7e    0xb2    0xc7
0x555555558029 <buffers+9>:     0xc6    0x98    0x95    0x65    0x12    0xee    0x45    0xe8
0x555555558031 <buffers+17>:    0xe4    0x22    0x84    0xc6    0xe9    0xb9    0xaa    0x61
0x555555558039 <buffers+25>:    0x73    0xd0    0x0f    0xb6    0xa6    0xde    0x06    0xd4
0x555555558041 <buffers+33>:    0xbb    0xe0    0x96    0x68    0xe8    0xc6    0x44    0x88
0x555555558049 <buffers+41>:    0x0a    0x30    0xc8    0x54    0x00    0x00    0x00    0x84      <================================= THIS IS THE END OF BUFFERS
0x555555558051 <final+1>:       0xe7    0xcc    0x30    0x10    0x7f    0xfe    0x40    0xe2
gef➤  x/40xb $rip+0x2dad
0x555555558049 <buffers+41>:    0x0a    0x30    0xc8    0x54    0x00    0x00    0x00    0x84    <==================================== START OF FINAL
0x555555558051 <final+1>:       0xe7    0xcc    0x30    0x10    0x7f    0xfe    0x40    0xe2
0x555555558059 <final+9>:       0x4d    0xb9    0x99    0x9c    0x18    0x03    0x00    0x80    <==================================== 0X3 END OF FINAL

-------------------------------------------------------------------------------------------------------------------------------------
Now that we know the array values and the algorithm we can reconstruct a script to mimic the checks.

script.py
-------------------------------------------------------------------------------------------------------------------------------------
from pwn import *

p =process()

# copied the hardcoded strings over from the binary
buffer = [0xa8,0x66,0xe5,0xa2,0xaf,0x8d,0x7e,0xb2,0xc7,0xc6,0x98,0x95,0x65,0x12,0xee,0x45,0xe8,0xe4,0x22,0x84,0xc6,0xe9,0xb9,0xaa,0x61,0x73,0xd0,0x0f,0xb6,0xa6,0xde,0x06,0xd4,0xbb,0xe0,0x96,0x68,0xe8,0xc6,0x44,0x88,0x0a,0x30,0xc8,0x54]
final = [0x84, 0xe7, 0xcc, 0x30, 0x10, 0x7f, 0xfe, 0x40, 0xe2, 0x4d, 0xb9, 0x99, 0x9c, 0x18, 0x03]
result = []

# we immitate the same hashing algorithm that we saw in the binary
for i in range(len(final)):
	char = final[14 - i] ^ buffer[(i%3 * 0xf + i)]
	char = hex(char)
	result.append(char)

# we now take the hex values and print it as one long string
print("\nThe string to submit is:\n")
j = 0
for byte in result:
	print (result[j][2:], end = '')
	j+=1
print("\n\n")

-------------------------------------------------------------------------------------------------------------------------------------
We get this:
-------------------------------------------------------------------------------------------------------------------------------------
[+] Starting local process '/home/kali/Hacking/cwe_screener_3.0/reversing/input2/input2': pid 2065297

The string to submit is:

abf0483b3ddb9cf938b9633aa951d0
-------------------------------------------------------------------------------------------------------------------------------------
lets make sure we append a null byte to the end of that and make sure to send it as bytes, and that should be our password
-------------------------------------------------------------------------------------------------------------------------------------
from pwn import *

context.binary = binary = ELF("./input2")

# p =process()
p = remote("www.cweaccessionsctf.com", 1405)

# copied the hardcoded strings over from the binary
buffer = [0xa8,0x66,0xe5,0xa2,0xaf,0x8d,0x7e,0xb2,0xc7,0xc6,0x98,0x95,0x65,0x12,0xee,0x45,0xe8,0xe4,0x22,0x84,0xc6,0xe9,0xb9,0xaa,0x61,0x73,0xd0,0x0f,0xb6,0xa6,0xde,0x06,0xd4,0xbb,0xe0,0x96,0x68,0xe8,0xc6,0x44,0x88,0x0a,0x30,0xc8,0x54]
final = [0x84, 0xe7, 0xcc, 0x30, 0x10, 0x7f, 0xfe, 0x40, 0xe2, 0x4d, 0xb9, 0x99, 0x9c, 0x18, 0x03]
result = []

# we immitate the same hashing algorithm that we saw in the binary
for i in range(len(final)):
	char = final[14 - i] ^ buffer[(i%3 * 0xf + i)]
	char = hex(char)
	result.append(char)

# we now take the hex values and print it as one long string
print("\nThe string to submit is:\n")
j = 0
for byte in result:
	print (result[j][2:], end = '')
	j+=1
print("\n\n")

# this was added after the fact 
password = "\xab\xf0\x48\x3b\x3d\xdb\x9c\xf9\x38\xb9\x63\x3a\xa9\x51\xd0\x00"


p.sendline(password)
p.interactive()

# flag{g3tt!ng_p@sT_$ecuritY_r_w3}

-------------------------------------------------------------------------------------------------------------------------------------
[*] Switching to interactive mode
Please enter the password: 
Congratulations!
flag{g3tt!ng_p@sT_$ecuritY_r_w3}
-------------------------------------------------------------------------------------------------------------------------------------
we did it!



