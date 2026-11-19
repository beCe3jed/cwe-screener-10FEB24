*Reverse Engineering & Offensive Security Portfolio*

A collection of independently completed reverse-engineering, binary-exploitation, and programming challenges from my preparation for the Cyber Warfare Engineer (CWE) screening process, July 2023 – February 2024.

This repository documents my approach to analyzing unfamiliar binaries, understanding low-level program behavior, developing exploits, and reconstructing algorithms using C, Python, and x86/x86-64 assembly.

Featured Projects

*[1. Assembly-Level Hash Algorithm Reconstruction](https://github.com/beCe3jed/cwe-screener-10FEB24/tree/main/reversing/hash)*

Skills: x86-64 Assembly, C, Reverse Engineering, Compiler Analysis
Analyzed x86-64 assembly to reconstruct the control flow and operations of a custom hashing algorithm.
Reimplemented the algorithm in C using pointer operations, conditional logic, and bitwise arithmetic.
Compiled the reconstructed implementation and achieved an exact match with the original assembly.
Key takeaway: Demonstrates the ability to translate machine-level instructions into equivalent high-level source code and verify the reconstruction through compiler output.

*[2. Stack Canary Bypass](https://github.com/beCe3jed/cwe-screener-10FEB24/tree/main/pwn/canary)*

Skills: Ghidra, GDB/GEF, Python, pwntools, Binary Exploitation
Analyzed an ELF binary to identify a stack-based buffer overflow and its protection mechanisms.
Investigated the stack-canary implementation through static and dynamic analysis.
Determined the relevant runtime values and stack offsets.
Developed a working exploit that preserved the expected canary while redirecting execution.
Key takeaway: Demonstrates vulnerability analysis, understanding of exploit mitigations, and practical exploit development.

*[3. Binary Input Validation Reverse Engineering](https://github.com/beCe3jed/cwe-screener-10FEB24/tree/main/reversing/input2)*

Skills: Ghidra, GDB/GEF, Python, Static and Dynamic Analysis
Analyzed decompiled functions to understand unfamiliar input-validation logic.
Inspected runtime memory to recover data used by the validation algorithm.
Reconstructed the relevant logic and developed a Python solution to generate valid inputs.
Key takeaway: Demonstrates algorithm reconstruction and combining static and dynamic analysis to understand compiled software.

*[4. Format-String Exploitation](https://github.com/beCe3jed/cwe-screener-10FEB24/tree/main/pwn/format)*

Skills: Python, pwntools, Ghidra, Memory Analysis
Identified exploitable format-string behavior in a compiled program.
Investigated stack values and program behavior to determine an information-disclosure approach.
Developed a working Python/pwntools exploit.
Key takeaway: Demonstrates vulnerability identification, memory analysis, and exploitation of format-string vulnerabilities.

*[5. Binary Instruction Interpreter](https://github.com/beCe3jed/cwe-screener-10FEB24/tree/main/Programming/interpreter)*

Skills: C, Binary File Parsing, Instruction Decoding, Bitwise Operations
Implemented a C-based interpreter for a custom binary instruction format, processing encoded operations involving control flow, arithmetic, byte manipulation, and bitwise transformations.
Key takeaway: Demonstrates practical C programming and understanding of instruction execution and low-level data representation.

*Additional Work*

Binary Exploitation Challenges — Buffer overflows, shellcode execution, and control-flow hijacking.
Reverse Engineering Challenges — Assembly analysis, binary inspection, and program-logic reconstruction.
C and Python Programming Challenges — Algorithms, data processing, memory management, and systems-oriented programming.

*Tools and Technologies*

Languages: C, Python, x86/x86-64 Assembly
Analysis: Ghidra, GDB/GEF, radare2
Exploitation: pwntools, ELF analysis, stack inspection, exploit-mitigation analysis

*Repository Organization*
Each challenge is organized in its respective directory. Available materials include source code, exploit scripts, analysis notes, and screenshots documenting the approach and solution.

All challenges were completed independently in an authorized screening/practice environment.
