lets get started with looking at this dissasembly:
------------------------------------------------------------------------------------------------------------------------------------------------------
0000000000001149 <hash>:
    1149:	f3 0f 1e fa          	endbr64 
    114d:	55                   	push   rbp
    114e:	48 89 e5             	mov    rbp,rsp
    1151:	48 89 7d d8          	mov    QWORD PTR [rbp-0x28],rdi
    1155:	66 c7 45 e8 00 00    	mov    WORD PTR [rbp-0x18],0x0
    115b:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [rbp-0x14],0x0
    1162:	48 8b 45 d8          	mov    rax,QWORD PTR [rbp-0x28]
    1166:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax
    116a:	eb 5f                	jmp    11cb <hash+0x82>           <--------------- JUMP #1
    116c:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    1170:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax
    1174:	8b 45 ec             	mov    eax,DWORD PTR [rbp-0x14]
    1177:	83 e0 01             	and    eax,0x1
    117a:	85 c0                	test   eax,eax
    117c:	75 22                	jne    11a0 <hash+0x57>             <--------------- JUMP #3
    117e:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
    1182:	0f b7 00             	movzx  eax,WORD PTR [rax]
    1185:	66 89 45 ea          	mov    WORD PTR [rbp-0x16],ax
    1189:	0f b7 45 ea          	movzx  eax,WORD PTR [rbp-0x16]
    118d:	66 69 c0 06 19       	imul   ax,ax,0x1906
    1192:	66 89 45 ea          	mov    WORD PTR [rbp-0x16],ax
    1196:	0f b7 45 ea          	movzx  eax,WORD PTR [rbp-0x16]
    119a:	66 31 45 e8          	xor    WORD PTR [rbp-0x18],ax
    119e:	eb 22                	jmp    11c2 <hash+0x79>             <--------------- JUMP #4
    11a0:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
    11a4:	0f b6 00             	movzx  eax,BYTE PTR [rax]
    11a7:	88 45 e7             	mov    BYTE PTR [rbp-0x19],al
    11aa:	0f b6 55 e7          	movzx  edx,BYTE PTR [rbp-0x19]
    11ae:	89 d0                	mov    eax,edx
    11b0:	c1 e0 03             	shl    eax,0x3
    11b3:	01 d0                	add    eax,edx
    11b5:	01 c0                	add    eax,eax
    11b7:	88 45 e7             	mov    BYTE PTR [rbp-0x19],al
    11ba:	0f b6 45 e7          	movzx  eax,BYTE PTR [rbp-0x19]
    11be:	66 31 45 e8          	xor    WORD PTR [rbp-0x18],ax
    11c2:	83 45 ec 01          	add    DWORD PTR [rbp-0x14],0x1
    11c6:	48 83 45 f0 01       	add    QWORD PTR [rbp-0x10],0x1
    11cb:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    11cf:	0f b6 00             	movzx  eax,BYTE PTR [rax]
    11d2:	84 c0                	test   al,al
    11d4:	75 96                	jne    116c <hash+0x23>           <--------------- JUMP #2: This is defintely some ype of check for a loop
    11d6:	0f b7 45 e8          	movzx  eax,WORD PTR [rbp-0x18]
    11da:	5d                   	pop    rbp
    11db:	c3                   	ret 
------------------------------------------------------------------------------------------------------------------------------------------------------
So we got a lot to look at... Lets split it up into sections of before each jump:
------------------------------------------------------------------------------------------------------------------------------------------------------
    1149:	f3 0f 1e fa          	endbr64 
    114d:	55                   	push   rbp 
    114e:	48 89 e5             	mov    rbp,rsp
    1151:	48 89 7d d8          	mov    QWORD PTR [rbp-0x28],rdi  <-- moves in the parameter
    1155:	66 c7 45 e8 00 00    	mov    WORD PTR [rbp-0x18],0x0   <-- declares a short h = 0  
    115b:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [rbp-0x14],0x0  <-- declares a int i = 0
    1162:	48 8b 45 d8          	mov    rax,QWORD PTR [rbp-0x28]  
    1166:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax  <-- declares a ptr address = our parameter
    116a:	eb 5f                	jmp    11cb <hash+0x82>           <--------------- JUMP #1
------------------------------------------------------------------------------------------------------------------------------------------------------
jumping to 11cb...
------------------------------------------------------------------------------------------------------------------------------------------------------
    11cb:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10] 
    11cf:	0f b6 00             	movzx  eax,BYTE PTR [rax]        <-- dereferences ptr address to a byte
    11d2:	84 c0                	test   al,al                     <-- performs an AND between the previous byte and itself... set zero flag if byte = 0
    11d4:	75 96                	jne    116c <hash+0x23>           <--------------- JUMP #2: checks zero flag, meaning will always jump until byte = 0
    11d6:	0f b7 45 e8          	movzx  eax,WORD PTR [rbp-0x18]
    11da:	5d                   	pop    rbp
    11db:	c3                   	ret           <--- return to parent function
------------------------------------------------------------------------------------------------------------------------------------------------------
So it's clear to see that our loop here stops when the byte is equal to zero. knowing that we will be building a hashing function, I think we can assume 
that this essentially checking for the end of the string to be hashed. Real quick, lets update the template.c we were given with what we know so far:
------------------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short hash(unsigned char * s){

    unsigned short h = 0;                                 #updated!
    unsigned int i = 0;                                   #updated!
    unsigned char *letter_address = s;                    #updated!
    
    while(*letter_address != 0){ // check if == '\0'      #updated!

    }
    return h;

}

int main(int argc, char * argv[]){

    for(int i = 1;i<argc;i++)
        printf("hash(\"%s\")=0x%04hx\n",argv[1], hash(argv[i])); 
}
------------------------------------------------------------------------------------------------------------------------------------------------------
Now lets break down what happens after JUMP #2:

116c....
------------------------------------------------------------------------------------------------------------------------------------------------------
    116c:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]    
    1170:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax      <-- we make a copy of our ptr address
    1174:	8b 45 ec             	mov    eax,DWORD PTR [rbp-0x14]      
    1177:	83 e0 01             	and    eax,0x1                      <-- performs AND on bottom byte of i
    117a:	85 c0                	test   eax,eax                      <-- sets ZF if i is odd
    117c:	75 22                	jne    11a0 <hash+0x57>             <--------------- JUMP #3 jump if i is odd
------------------------------------------------------------------------------------------------------------------------------------------------------
Pretty simple so far. Lets update our program:
------------------------------------------------------------------------------------------------------------------------------------------------------
short hash(unsigned char * s){

    unsigned short h = 0;
    unsigned int i = 0;
    unsigned char *letter_address = s;
    
    while(*letter_address != 0){ // check if == '\0'
         unsigned char *copy_address = (unsigned char*)letter_address;      # updated   
        if((i % 2) != 0) {                                # updated   
        }
        else {                                            # updated
        }
    }
    return h;

}
------------------------------------------------------------------------------------------------------------------------------------------------------
Now lets assume that i is odd 

11a0...
------------------------------------------------------------------------------------------------------------------------------------------------------
    11a0:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]    
    11a4:	0f b6 00             	movzx  eax,BYTE PTR [rax]        <-- dereferences the byte from our address
    11a7:	88 45 e7             	mov    BYTE PTR [rbp-0x19],al    <-- constructs a new ptr to it 
    11aa:	0f b6 55 e7          	movzx  edx,BYTE PTR [rbp-0x19]    
    11ae:	89 d0                	mov    eax,edx                    <-- copy the original byte value
    11b0:	c1 e0 03             	shl    eax,0x3                    <-- shift the copied byte 3 bits to the left
    11b3:	01 d0                	add    eax,edx                    <-- add the original byte into the shifted byte
    11b5:	01 c0                	add    eax,eax                    <-- double the copied byte
    11b7:	88 45 e7             	mov    BYTE PTR [rbp-0x19],al      <-- only copy the lowest 8 bits back into the ptr
    11ba:	0f b6 45 e7          	movzx  eax,BYTE PTR [rbp-0x19]
    11be:	66 31 45 e8          	xor    WORD PTR [rbp-0x18],ax      <-- we now need to XOR h with our newly manipulated address as a !SHORT!
    11c2:	83 45 ec 01          	add    DWORD PTR [rbp-0x14],0x1    <-- increment i
    11c6:	48 83 45 f0 01       	add    QWORD PTR [rbp-0x10],0x1    <-- increment our address ptr... essentially move to next byte in string

  # --------- This is repeated from before --------------#
    11cb:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    11cf:	0f b6 00             	movzx  eax,BYTE PTR [rax]
    11d2:	84 c0                	test   al,al
    11d4:	75 96                	jne    116c <hash+0x23>           <--------------- JUMP #2: This is defintely some ype of check for a loop
------------------------------------------------------------------------------------------------------------------------------------------------------
Okay so here we do some shifting and mathematical operations on a byte, but an important thing to notice is that we originally reference the address as
a BYTE ptr but then later reference it as a WORD ptr (short). This had tricked me for a while because we cannot simply cast the char* into a short*. I 
realized that in order to be flexible with what type of pointer we want it to be, we will have to set the copy address to a void*.

let's update our program
------------------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short hash(unsigned char * s){

    unsigned short h = 0;
    unsigned int i = 0;
    unsigned char *letter_address = s;
    
    while(*letter_address != 0){ // check if == '\0'
         void *copy_address = (void*)letter_address;                                  # edited
        if((i % 2) == 0) {
        }
        else {
            // if odd
            unsigned char copy_letter = *(unsigned char*)copy_address;                # updated
            copy_letter = (unsigned char)(((copy_letter * 8) + copy_letter) * 2);     # updated
            h = h ^ (unsigned short)copy_letter;                                      # updated
        }
        i++;                                                                          # updated
        letter_address++;                                                             # updated
    }
    return h;

}

int main(int argc, char * argv[]){

    for(int i = 1;i<argc;i++)
        printf("hash(\"%s\")=0x%04hx\n",argv[1], hash(argv[i])); 
}

------------------------------------------------------------------------------------------------------------------------------------------------------
now lets look at what would happen if i was even

117e...
------------------------------------------------------------------------------------------------------------------------------------------------------
    117e:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]     <-- make a copy of our address
    1182:	0f b7 00             	movzx  eax,WORD PTR [rax]          <-- dereference it into a short
    1185:	66 89 45 ea          	mov    WORD PTR [rbp-0x16],ax    
    1189:	0f b7 45 ea          	movzx  eax,WORD PTR [rbp-0x16]      <--make a copy of the short
    118d:	66 69 c0 06 19       	imul   ax,ax,0x1906                <-- multiply the copy by 6406
    1192:	66 89 45 ea          	mov    WORD PTR [rbp-0x16],ax      
    1196:	0f b7 45 ea          	movzx  eax,WORD PTR [rbp-0x16]
    119a:	66 31 45 e8          	xor    WORD PTR [rbp-0x18],ax      <-- XOR the h with our multiplied byte
    119e:	eb 22                	jmp    11c2 <hash+0x79>             <--------------- JUMP #4
#------------------ JUMPING TO 11c2 -----------------------#
#------------------ We've seen this before  -----------------------#
    11c2:	83 45 ec 01          	add    DWORD PTR [rbp-0x14],0x1
    11c6:	48 83 45 f0 01       	add    QWORD PTR [rbp-0x10],0x1
    11cb:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    11cf:	0f b6 00             	movzx  eax,BYTE PTR [rax]
    11d2:	84 c0                	test   al,al
    11d4:	75 96                	jne    116c <hash+0x23>           <--------------- JUMP #2: This is defintely some ype of check for a loop
------------------------------------------------------------------------------------------------------------------------------------------------------
Essentially we just make a copy of a short value from our address, multiply it by 6406 and XOR our h with this outcome.
Then we just iterate our i value, and increment our string pointer

updating our program...
------------------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short hash(unsigned char * s){

    unsigned short h = 0;
    unsigned int i = 0;
    unsigned char *letter_address = s;
    
    while(*letter_address != 0){ // check if == '\0'
         void *copy_address = (void*)letter_address;
        if((i % 2) == 0) {                                                   # updated
            // if i is even 
            unsigned short multiply = *(unsigned short*)copy_address;        # updated
            multiply *= 6406;                                                # updated
            h = h ^ multiply;                                                # updated
        }
        else {
            // if odd
            unsigned char copy_letter = *(unsigned char*)copy_address;
            copy_letter = (unsigned char)(((copy_letter * 8) + copy_letter) * 2);
            h = h ^ (unsigned short)copy_letter;
        }
        // printf("iteration #%d, letter: %c, val: %d\n", i, *letter_address, h);
        i++;
        letter_address++;
    }
    return h;

}

int main(int argc, char * argv[]){

    for(int i = 1;i<argc;i++)
        printf("hash(\"%s\")=0x%04hx\n",argv[1], hash(argv[i])); 
}
------------------------------------------------------------------------------------------------------------------------------------------------------
We are able to decompile this function using gdb and it looks like we get an exact match!

gdb:
------------------------------------------------------------------------------------------------------------------------------------------------------
gef➤  disassemble hash 
Dump of assembler code for function hash:
   0x0000000000001139 <+0>:     push   rbp
   0x000000000000113a <+1>:     mov    rbp,rsp
   0x000000000000113d <+4>:     mov    QWORD PTR [rbp-0x28],rdi
   0x0000000000001141 <+8>:     mov    WORD PTR [rbp-0x2],0x0
   0x0000000000001147 <+14>:    mov    DWORD PTR [rbp-0x8],0x0
   0x000000000000114e <+21>:    mov    rax,QWORD PTR [rbp-0x28]
   0x0000000000001152 <+25>:    mov    QWORD PTR [rbp-0x10],rax
   0x0000000000001156 <+29>:    jmp    0x11b7 <hash+126>
   0x0000000000001158 <+31>:    mov    rax,QWORD PTR [rbp-0x10]
   0x000000000000115c <+35>:    mov    QWORD PTR [rbp-0x18],rax
   0x0000000000001160 <+39>:    mov    eax,DWORD PTR [rbp-0x8]
   0x0000000000001163 <+42>:    and    eax,0x1
   0x0000000000001166 <+45>:    test   eax,eax
   0x0000000000001168 <+47>:    jne    0x118c <hash+83>
   0x000000000000116a <+49>:    mov    rax,QWORD PTR [rbp-0x18]
   0x000000000000116e <+53>:    movzx  eax,WORD PTR [rax]
   0x0000000000001171 <+56>:    mov    WORD PTR [rbp-0x1c],ax
   0x0000000000001175 <+60>:    movzx  eax,WORD PTR [rbp-0x1c]
   0x0000000000001179 <+64>:    imul   ax,ax,0x1906
   0x000000000000117e <+69>:    mov    WORD PTR [rbp-0x1c],ax
   0x0000000000001182 <+73>:    movzx  eax,WORD PTR [rbp-0x1c]
   0x0000000000001186 <+77>:    xor    WORD PTR [rbp-0x2],ax
   0x000000000000118a <+81>:    jmp    0x11ae <hash+117>
   0x000000000000118c <+83>:    mov    rax,QWORD PTR [rbp-0x18]
   0x0000000000001190 <+87>:    movzx  eax,BYTE PTR [rax]
   0x0000000000001193 <+90>:    mov    BYTE PTR [rbp-0x19],al
   0x0000000000001196 <+93>:    movzx  edx,BYTE PTR [rbp-0x19]
   0x000000000000119a <+97>:    mov    eax,edx
   0x000000000000119c <+99>:    shl    eax,0x3
   0x000000000000119f <+102>:   add    eax,edx
   0x00000000000011a1 <+104>:   add    eax,eax
   0x00000000000011a3 <+106>:   mov    BYTE PTR [rbp-0x19],al
   0x00000000000011a6 <+109>:   movzx  eax,BYTE PTR [rbp-0x19]
   0x00000000000011aa <+113>:   xor    WORD PTR [rbp-0x2],ax
   0x00000000000011ae <+117>:   add    DWORD PTR [rbp-0x8],0x1
   0x00000000000011b2 <+121>:   add    QWORD PTR [rbp-0x10],0x1
   0x00000000000011b7 <+126>:   mov    rax,QWORD PTR [rbp-0x10]
   0x00000000000011bb <+130>:   movzx  eax,BYTE PTR [rax]
   0x00000000000011be <+133>:   test   al,al
   0x00000000000011c0 <+135>:   jne    0x1158 <hash+31>
   0x00000000000011c2 <+137>:   movzx  eax,WORD PTR [rbp-0x2]
   0x00000000000011c6 <+141>:   pop    rbp
   0x00000000000011c7 <+142>:   ret
------------------------------------------------------------------------------------------------------------------------------------------------------
after running it with our text from text.txt, we get this....
------------------------------------------------------------------------------------------------------------------------------------------------------
┌──(kali㉿kali)-[~/Hacking/cwe_screener_2.0/reversing/Hash]
└─$ ./final "Developing a compiler was a logical move; but in matters like this, you don't run against logic - you run against people who can't change their minds. -Grace Hopper"
hash("Developing a compiler was a logical move; but in matters like this, you don't run against logic - you run against people who can't change their minds. -Grace Hopper")=0xcbe0
------------------------------------------------------------------------------------------------------------------------------------------------------
our hash is 0xcbe0 !



