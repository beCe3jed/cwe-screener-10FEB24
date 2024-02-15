
final = 0xb88202
print(hex(final))

tier1 = final ^ 0x6f0 	# 0xb88442
print(hex(tier1))

tier2 = tier1 << 4 		# 0xb884420
print(hex(tier2))

myInput = tier2 ^ 0x735	# 0xb884315
print("Your input should be: " + str(myInput))
