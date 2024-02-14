from pwn import *

context.binary = binary = ELF("./input2")

p = process()

password = b"0x45"