from pwn import *

context.binary = binary = ELF("./input1")

p = process()

'''
answer = 0xb88202

(myInput ^ 0x735) >> 4 ^ 0x640 = 0xb88202

0x640 ^ 0xb88202 = 0xb88442

0xb88442 << 4 = 0xb88442 = 0xb884420

0xb884420 ^ 0x735 = 0xB884815


PASSWORD IS 193480725

FLAG IS: flag{3sNt_a$$embly_sw33T}
'''
myInput = p64(0xB884315)

print(myInput)

p.sendline(myInput)

p.interactive()