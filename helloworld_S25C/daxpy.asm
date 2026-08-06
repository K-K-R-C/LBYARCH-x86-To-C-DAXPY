%include "io64.inc"

section .data

section .text

global daxpy

daxpy:
	;void daxpy(int n, double A, double* X, double* Y, double* Z);
	;RCX = n
	;XMM1 = A
	;R8 = X
	;R9 = Y
	;Z = answer (stack)

	MOV R10, [RSP + 40] ;R10 has Z address (the 5th argument)
	MOVSD XMM3, XMM1 ;to keep copy for A

	LOOP1:
		CMP RCX, 0
		JE FINISH1

		;To get X[i] and multiply with A
		MOVSD XMM0, [R8]
		MULSD XMM0, XMM3

		;To get Y[i] and add it to the multiplied result, then store to Z[i]
		MOVSD XMM2, [R9]
		ADDSD XMM0, XMM2
		MOVSD [R10], XMM0

		;So X/Y/Z pointers point to next element in array (Double = 8 bytes)
		ADD R8, 8
		ADD R9, 8
		ADD R10, 8

		DEC RCX
		JMP LOOP1

	FINISH1:

	RET