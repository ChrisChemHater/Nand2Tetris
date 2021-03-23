// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

// for (int i=R1; i>0; i--) {
//     R2 += R0;
// }

// R2=0
// i=R1
@R2
M=0
@R1
D=M
@i
M=D

// (LOOP)
// i=0? GOTO ENDLOOP
// R2=R2+R0
// i=i-1
// GOTO LOOP
(LOOP)
    @i
    D=M
    @ENDLOOP
    D;JEQ
    @R0
    D=M
    @R2
    M=D+M
    @i
    M=M-1
    @LOOP
    0;JMP

// (ENDLOOP)
// GOTO ENDLOOP
(ENDLOOP)
    @ENDLOOP
    0;JMP