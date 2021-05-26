.386
.model flat,stdcall
option casemap:none

include msvcrt.inc
includelib		msvcrt.lib
printf			PROTO C :ptr sbyte, :VARARG

.data			
N				DWORD	2
flag			BYTE	0
string_a		DB		"%d^2 + %d^2 + %d^2 + %d^2 = %d", 0AH, 0
string_n		DB		"No solution", 0AH, 0
szPause			DB		"pause", 0

.code
main			proc
				LOCAL	i, j, w, y, ans : DWORD
				MOV		eax,	N
L0:
				MOV		i,		0
				JMP		L2
L1:				
				INC		i
L2:
				MOV		ebx,	i
				IMUL	ebx,	i
				CMP		ebx,	eax
				JGE		L20
L3:
				MOV		j,		0
				JMP		L5
L4:
				INC		j
L5:				
				MOV		ebx,	j
				IMUL	ebx,	j
				CMP		ebx,	eax
				JGE		L1
L6:
				MOV		w,		0
				JMP		L8
L7:
				INC		w
L8:
				MOV		ebx,	w
				IMUL	ebx,	w
				CMP		ebx,	eax
				JGE		L4
L9:
				MOV		y,		0
				JMP		L11
L10:
				INC		y
L11:
				MOV		ebx,	y
				IMUL	ebx,	y
				CMP		ebx,	eax
				JGE		L7
L12:
				MOV		ebx,	i
				IMUL	ebx,	i
				MOV		ecx,	j
				IMUL	ecx,	j
				ADD		ebx,	ecx
				MOV		edx,	w
				IMUL	edx,	w
				ADD		ebx,	edx
				MOV		ecx,	y
				IMUL	ecx,	y
				ADD		ebx,	ecx
				MOV		ans,	ebx
L13:
				MOV		ebx,	ans
				CMP		ebx,	eax
				JNE		L16
L14:
				PUSH	eax
				INVOKE	printf,	 offset	 string_a,	 i, j, w, y, ans
				POP		eax
L15:
				MOV		BYTE PTR [flag],	1
L16:
				JMP		L10
L17:
				JMP		L7
L18:
				JMP		L4
L19:
				JMP		L1
L20:
				MOV		bl,	flag
				test	bl,	bl
				JNE		L22
L21:
				INVOKE	printf,	 offset	string_n
L22:
				INVOKE   crt_system, offset szPause
L23:
				XOR		eax,	eax
				ret
main			endp
end				main





				