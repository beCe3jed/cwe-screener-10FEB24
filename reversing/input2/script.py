from pwn import *

context.binary = binary = ELF("./input2")

# p = process()

# p1 = p64(0xf938b9633aa951d0)
# p2 = p64(0xabf0483b3ddb9c)
# p3 = b"\xd0\x51\xa9\x3a\x63\xb9\x38\xf9\x9c\xdb\x3d\x3b\x48\xf0\xab\x00"

p = remote("www.cweaccessionsctf.com", 1405)

p4 = b"\xab\xf0\x48\x3b\x3d\xdb\x9c\xf9\x38\xb9\x63\x3a\xa9\x51\xd0\x00"

password = p4

# file_path = 'password.bin'

# with open(file_path, 'wb') as file:
#     file.write(password)


# print(password)
# print(p3)

p.sendline(password)
p.interactive()

# flag{g3tt!ng_p@sT_$ecuritY_r_w3}