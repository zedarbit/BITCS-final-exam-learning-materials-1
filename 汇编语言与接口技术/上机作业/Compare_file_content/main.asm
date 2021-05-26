.386
.model flat,stdcall
option casemap:none

include msvcrt.inc
includelib		msvcrt.lib
include kernel32.inc
includelib		kernel32.lib
include user32.inc
includelib		user32.lib
include masm32rt.inc
includelib masm32rt.lib

;fopen			PROTO C :dword, :dword
fgets			PROTO C :dword, :dword,:dword
;fclose			PROTO C :dword
sprintf			PROTO C :ptr sbyte, :ptr sbyte, :VARARG

.data
hInstance		dd		?		;应用程序的句柄
hWinMain		dd		?		;窗口的句柄
showButton		byte	'SURE', 0
button			byte	'button', 0
showEdit1		byte	'FILE1:', 0
showEdit2		byte	'FILE2:', 0
edit			byte	'edit', 0
filePath1		byte	256	DUP(?)
filePath2		byte	256	DUP(?)
openWay			byte    'r',	0
diffNum			dd		?
diffOut			byte	2000 DUP(0)
buff1			byte	1024 DUP(0)
buff2			byte	1024 DUP(0)
hwndEdit1		HWND	?
hwndEdit2		HWND	?
TestString		DB		'Input file path ^_^',	0

.const
winClassName	db		'MyClass', 0
winCaptionName	db		'Compare File', 0
winMessage		db		'Input file Path', 0
MAXSIZE			dd		1024
SameContent		db		'there is no different _line between file1 and file2', 0
DiffContent		db		'different line: %d',0AH,0
szBoxTitle		db		'Compare Outcome',0


.code
_ReadLine		PROC	USES	ebx,	fp:	HANDLE,	buff:ptr byte
				LOCAL	lpNum:	dword
				LOCAL	_str:	BYTE

				MOV		ebx,	buff
				.while	TRUE
					INVOKE	ReadFile,	fp,	addr _str,	1,	addr lpNum,	NULL
					.break	.if	! lpNum
					.break	.if	_str==10

					MOV		al,	_str
					MOV		[ebx],	al
					INC		ebx
				.endw
				MOV		al,		0
				MOV		[ebx],	al

				INVOKE	lstrlen,	buff
				RET
_ReadLine		ENDP


_CompareFile	PROC	USES	eax ebx ecx lpFilePath1: PTR BYTE, lpFilePath2:	PTR BYTE
				LOCAL	fp1		:HANDLE
				LOCAL	fp2		:HANDLE
				LOCAL	__line	:DWORD
				LOCAL	diffTem[1000] :BYTE
				LOCAL	p1		:DWORD
				LOCAL	p2		:DWORD

				INVOKE	CreateFile, lpFilePath1, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
				MOV		fp1,	eax
				INVOKE	CreateFile, lpFilePath2, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
				MOV		fp2,	eax

				MOV		__line,	0
				MOV		diffNum,0

CYCLE:				;   while (true) {
				XOR		ebx,	ebx
				CMP		ebx,	1
				JE		_CLOSE

				MOV		[diffTem],	0
				INC		__line
				INVOKE	RtlZeroMemory,		addr	buff1,	SIZEOF	MAXSIZE		;
				INVOKE	RtlZeroMemory,		addr	buff1,	SIZEOF	MAXSIZE		;

				;	char* p1 = fgets(buff1, MAXSIZE, fp1);
				;	char* p2 = fgets(buff2, MAXSIZE, fp2);
				invoke	_ReadLine,	fp1,	OFFSET buff1
				MOV		p1,		eax
				invoke	_ReadLine,	fp2,	OFFSET buff2
				MOV		p2,		eax

L1:				;	if (p1 == NULL) {
				CMP		p1,		0
				JNE		L2
				;		if (p2 == NULL) {
				CMP		p2,		0
							;break
				JE		_CLOSE

				INVOKE	sprintf,	ADDR diffTem,	OFFSET	DiffContent,	__line
				INVOKE	lstrcat,	ADDR diffOut,	ADDR	diffTem
				INC		diffNum
				JMP		CYCLE

L2:				;	else {
				;		if (p2 == NULL) {
				CMP		p2,		0
				JNE		L21
L20:
				INVOKE	sprintf,	ADDR diffTem,	OFFSET	DiffContent,	__line
				INVOKE	lstrcat,	ADDR diffOut,	ADDR	diffTem
				INC		diffNum
				JMP		CYCLE
L21:			;else {
				;	int ans = strcmp(buff1, buff2);
				INVOKE	lstrcmp,	OFFSET	buff1,	OFFSET	buff2
				;				if (ans != 0) {
				CMP		eax,	0
				JE		CYCLE

				INVOKE	sprintf,	ADDR diffTem,	OFFSET	DiffContent,	__line
				INVOKE	lstrcat,	ADDR diffOut,	ADDR	diffTem
				INC		diffNum
				JMP		CYCLE

_CLOSE:
				;INVOKE	fclose,	OFFSET	fp1
				fclose	fp1
				fclose	fp2
				;INVOKE	fclose,	OFFSET	fp2
				XOR		eax,	eax
				RET
_CompareFile	ENDP


_ProcWinMain	PROC	USES ebx edi esi, hWnd, uMsg, wParam, lParam   ;窗口过程
				LOCAL	structPs:		PAINTSTRUCT
				LOCAL	structRect:	RECT
				LOCAL	hDc

				MOV		eax,	uMsg

				.IF		eax==WM_PAINT		
						INVOKE			BeginPaint, hWnd,	addr structPs
						MOV				hDc,		eax
						INVOKE			EndPaint,	hWnd,	addr structPs
				
				.ELSEIF	eax==WM_CLOSE   ;窗口关闭信息
						INVOKE			DestroyWindow, hWinMain
						INVOKE			PostQuitMessage, NULL

				.ELSEIF	eax==WM_CREATE    ;创建窗口
						INVOKE			CreateWindowEx,	WS_EX_CLIENTEDGE, OFFSET edit, NULL,\
														WS_CHILD OR  WS_VISIBLE	OR WS_BORDER OR	ES_LEFT	OR	ES_AUTOHSCROLL,\
														10, 10, 350, 35,\
														hWnd,	1,		hInstance,	NULL   
						MOV				hwndEdit1,		eax							
						INVOKE			CreateWindowEx,	WS_EX_CLIENTEDGE, OFFSET edit, NULL,\
														WS_CHILD OR  WS_VISIBLE	OR WS_BORDER OR	ES_LEFT	OR	ES_AUTOHSCROLL,\
														10, 50, 350, 35,\
														hWnd,	2,		hInstance,	NULL 
						MOV				hwndEdit2,		eax		
						INVOKE			CreateWindowEx,	NULL, OFFSET button, OFFSET showButton,\
														WS_CHILD OR  WS_VISIBLE,	220, 100, 200, 45,\
														hWnd,	3,		hInstance,	NULL    
						

				.ELSEIF	eax==WM_COMMAND
						mov eax,wParam  ;其中参数wParam里存的是句柄，如果点击了一个按钮，则wParam是那个按钮的句柄
						.IF	eax ==3
							invoke GetWindowText,hwndEdit1,ADDR filePath1,512
							invoke GetWindowText,hwndEdit2,ADDR filePath2,512
							INVOKE			_CompareFile,	offset	filePath1,	OFFSET filePath2
								.IF	diffNum == 0
									INVOKE		MessageBox,	hWnd,	OFFSET SameContent,	OFFSET szBoxTitle,	MB_OK + MB_ICONQUESTION
								.ELSE
									;输出不同的行数
									INVOKE		MessageBox,	hWnd,	OFFSET diffOut,	OFFSET szBoxTitle,	MB_OK + MB_ICONQUESTION
								.ENDIF
						.ENDIF


				.ELSE   
						INVOKE			DefWindowProc, hWnd, uMsg, wParam, lParam
						RET
				.ENDIF

				XOR		eax,			eax
				RET
_ProcWinMain	ENDP


_WinMain		PROC
				LOCAL	structWndClass:	WNDCLASSEX	;定义一个WNDCLASSEX类型的结构变量
				LOCAL	structMsg:			MSG		;定义一个MSG类型变量，用于消息传递
				
				INVOKE	GetModuleHandle,	NULL	;得到应用程序的句柄
				MOV		hInstance,			eax
				
				INVOKE	RtlZeroMemory,		addr		structWndClass,		sizeof	structWndClass		;将structWndClaa初始化为零
				INVOKE	LoadCursor,			0,			IDC_ARROW
				MOV		structWndClass.hCursor,		eax
				push	hInstance
				pop		structWndClass.hInstance
				MOV		structWndClass.cbSize,			sizeof	WNDCLASSEX
				MOV		structWndClass.style,			CS_HREDRAW			OR		CS_VREDRAW
				MOV		structWndClass.lpfnWndProc,	offset	_ProcWinMain	;指定该窗口程序的窗口过程是_ProcWinMain
				MOV		structWndClass.hbrBackground,	COLOR_WINDOW + 1
				MOV		structWndClass.lpszClassName,	offset	winClassName
				INVOKE	RegisterClassEx,	addr		structWndClass		;先填写WNDCLASSEX的结构再注册

				INVOKE	CreateWindowEx,		WS_EX_CLIENTEDGE,	offset winClassName,	offset winCaptionName,\
						WS_OVERLAPPEDWINDOW, 100, 100, 500, 200, NULL, NULL, hInstance, NULL
				MOV		hWinMain,			eax

				INVOKE	ShowWindow,			hWinMain,	SW_SHOWNORMAL	;显示窗口
				INVOKE	UpdateWindow,		hWinMain		;刷新窗口客户区

				.WHILE TRUE    ;进入无限循环的消息获取和处理
						INVOKE				GetMessage,	addr	structMsg,	NULL,	0,	0
						.break				.IF			eax==0   ;若推出循环，eax置为零
						INVOKE				TranslateMessage,	addr	structMsg		;将基于键盘扫描码的按键信息转换成对应的ASCII码，若消息非键盘输入，这步跳过
						INVOKE				DispatchMessage,	addr	structMsg		;通过该窗口的窗口过程处理消息
				.ENDW
				RET
_WinMain		ENDP

main			PROC
				CALL	_WinMain
				INVOKE	ExitProcess,	NULL
				RET
main			ENDP
END			main
