.386
.model flat,stdcall
option casemap:none

include msvcrt.inc
includelib		msvcrt.lib
printf			PROTO C :ptr sbyte, :VARARG
scanf			PROTO C :ptr sbyte, :VARARG
strlen			PROTO C :ptr sbyte

.data	
snum1			BYTE		200		DUP(0)
snum2			BYTE		200		DUP(0)
num1			DWORD	200		DUP(0)
num2			DWORD	200		DUP(0)
ans				DWORD	210		DUP(0)
len				DWORD	0
len1			DWORD	0
len2			DWORD	0
string_in		DB		"%s",	0
string_ans		DB		"%d",	0
string_n		DB		0aH,	0
szPause			DB		"pause", 0

.code
main			proc
				LOCAL	carry	:DWORD
L0:
				INVOKE	scanf,	offset string_in,	OFFSET snum1      
				INVOKE	strlen,	offset snum1						;	const int len1 = strlen(snum1);
				MOV		len1,	eax
				INVOKE	scanf,	offset string_in,	offset snum2
				INVOKE	strlen,	offset snum2                     ;	const int len2 = strlen(snum2);
				MOV		len2,	eax
L1:
				;for (int i = 0, j = len1 - 1; i < len1; i++, j--) 
				XOR		esi,	esi
				MOV		edi,		len1
				DEC		edi
				JMP		L3
L2:
				INC		esi
				DEC		edi
L3:
				MOV		ecx,	len1
				CMP		esi,	ecx
				JGE		L5
L4:
				;num1[i] = snum1[j] - '0';
				MOVSX	ebx,	snum1[edi]
				SUB		ebx,	48
				MOV		num1[esi*4],	ebx
				JMP		L2
L5:
				;for (int i = 0, j = len2 - 1; i < len2; i++, j--) 
				XOR		esi,	esi
				MOV		edi,	len2
				DEC		edi
				JMP		L7
L6:
				INC		esi
				DEC		edi
L7:
				MOV		ecx,	len2
				CMP		esi,	ecx
				JGE		L9
L8:
				;num2i] = snum2[j] - '0';
				MOVSX	ebx,	snum2[edi*1]
				SUB		ebx,	48
				MOV		num2[esi*4],	ebx
				JMP		L6
L9:
				;for (int i = 0; i < len1; i++) {
				XOR		esi,	esi
				JMP		L11
L10:
				INC		esi
L11:
				MOV		ebx,	len1
				CMP		esi,	ebx
				JGE		L16
L12:
				;	for (int j = 0; j < len2; j++) {
				XOR		edi,	edi
				JMP		L14
L13:
				INC		edi
L14:
				MOV		ebx,	len2
				CMP		edi,	ebx
				JGE		L10
L15:
				;		ans[i+j] += num1[i]*num2[j]
				MOV		ebx,	num1[esi*4]
				IMUL	ebx,	num2[edi*4]
				MOV		ecx,	esi
				ADD		ecx,	edi
				ADD		ans[ecx*4],	ebx	
				JMP		L13
L16:
				;len = len1 + len2 - 2
				MOV		ebx,	len1
				ADD		ebx,	len2
				SUB		ebx,	2
				MOV		len,	ebx
L17:
				;for (int i = 0; i <= len; i++)
				XOR		esi,	esi
				JMP		L19
L18:
				INC		esi
L19:
				MOV		ebx,	len
				CMP		esi,	ebx
				JG		L25
L20:
				;	carry = ans[i]/10
				XOR		edx,	edx
				MOV		eax,	ans[esi*4]
				MOV		ecx,	10
				IDIV	ecx
				MOV		carry,	eax
L21:
				;	a[i] %= 10
				XOR		edx,	edx
				MOV		eax,	ans[esi*4]
				MOV		ecx,	10
				IDIV	ecx
				MOV		ans[esi*4],	edx
L22:
				;ans[i+1]+=carry
				MOV		ebx,	esi
				INC		ebx
				MOV		ecx,	ans[ebx*4]
				ADD		ecx,	carry
				MOV		ans[ebx*4],	ecx
L23:
				;if (i == len - 1 && carry != 0)
				MOV		ebx,	len
				CMP		ebx,	esi
				JNE		L18
				CMP		carry,	0
				JE		L18
L24:
				;len++
				MOV		ebx,	len
				INC		ebx
				MOV		len,	ebx
				JMP		L18
L25:
				;for (int i = len; i >= 0; i--)
				MOV		esi,	len
				JMP		L27
L26:
				DEC		esi
L27:
				CMP		esi,	0
				JL		L29
L28:
				;	printf("%d",ans[i])
				INVOKE	printf,	offset string_ans,	ans[esi*4]
				JMP		L26
L29:
				;printf("\n")
				INVOKE	printf,	offset string_n
				INVOKE   crt_system, offset szPause

				XOR		eax,	eax
				XOR		esi,	esi
				XOR		edi,	edi
				ret
main			endp
end				main