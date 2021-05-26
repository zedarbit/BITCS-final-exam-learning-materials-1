.386
.model flat,stdcall
option casemap:none

include windows.inc
include gdi32.inc
includelib gdi32.lib
include user32.inc
includelib user32.lib
include kernel32.inc
includelib kernel32.lib

.data
hInstance dd ?  ;存放应用程序的句柄
hWinMain dd ?   ;存放窗口的句柄
showButton byte 'button',0
button db 'button',0

.const
szClassName db 'MyClass',0
szCaptionMain db 'My first Window!',0
szText db 'Win32 Assembly,Simple and powerful!',0

.code

_ProcWinMain proc uses ebx edi esi,hWnd,uMsg,wParam,lParam  ;窗口过程
	local @stPs:PAINTSTRUCT
	local @stRect:RECT
	local @hDc

	mov eax,uMsg  ;uMsg是消息类型，如下面的WM_PAINT,WM_CREATE

	.if eax==WM_PAINT  ;如果想自己绘制客户区，在这里些代码，即第一次打开窗口会显示什么信息
		invoke BeginPaint,hWnd,addr @stPs
		mov @hDc,eax

		invoke GetClientRect,hWnd,addr @stRect
		invoke DrawText,@hDc,addr szText,-1,addr @stRect,DT_SINGLELINE or DT_CENTER or DT_VCENTER  ;这里将显示szText里的字符串
		invoke EndPaint,hWnd,addr @stPs
	
	.elseif eax==WM_CLOSE  ;窗口关闭消息
		invoke DestroyWindow,hWinMain
		invoke PostQuitMessage,NULL

	.elseif eax==WM_CREATE  ;创建窗口  下面代码表示创建一个按钮，其中button字符串值是'button'，在数据段定义，表示要创建的是一个按钮，showButton表示该按钮上的显示信息
		invoke CreateWindowEx,NULL,offset button,offset showButton,\
		WS_CHILD or WS_VISIBLE,10,10,200,30,\  ;10，10，200，30代表按钮尺寸大小和坐标等。。。
		hWnd,1,hInstance,NULL  ;1表示该按钮的句柄是1

	.elseif eax==WM_COMMAND  ;点击时候产生的消息是WM_COMMAND
		mov eax,wParam  ;其中参数wParam里存的是句柄，如果点击了一个按钮，则wParam是那个按钮的句柄
		.if eax==1  ;接着则判断句柄是多少得知是哪个按钮被按下了，从而做相应的操作，这个例子是句柄为1的按钮被按下，这将创建一个句柄为2的按钮
			invoke CreateWindowEx,NULL,offset button,offset showButton,\
			WS_CHILD or WS_VISIBLE,100,100,200,30,\ 
			hWnd,2,hInstance,NULL 
		.endif

	;----------------------
	;显然这这部分是自己添加的相应处理事件的代码，如添加某个按钮，点击该按钮会发生什么事等。
	;还有其他的消息类型如WM_CREATE，代表窗口创建时，WM_COMMAND表示点击按钮时,在这里添加分支，编写相应的处理事件的代码
	;----------------------

	.else  ;否则按默认处理方法处理消息
		invoke DefWindowProc,hWnd,uMsg,wParam,lParam
		ret
	.endif

	xor eax,eax
	ret
_ProcWinMain endp

_WinMain proc  ;窗口程序
	local @stWndClass:WNDCLASSEX  ;定义了一个结构变量，它的类型是WNDCLASSEX，一个窗口类定义了窗口的一些主要属性，图标，光标，背景色等，这些参数不是单个传递，而是封装在WNDCLASSEX中传递的。
	local @stMsg:MSG	;还定义了stMsg，类型是MSG，用来作消息传递的	

	invoke GetModuleHandle,NULL  ;得到应用程序的句柄，把该句柄的值放在hInstance中，句柄是什么？简单点理解就是某个事物的标识，有文件句柄，窗口句柄，可以通过句柄找到对应的事物
	mov hInstance,eax
	invoke RtlZeroMemory,addr @stWndClass,sizeof @stWndClass  ;将stWndClass初始化全0

	invoke LoadCursor,0,IDC_ARROW
	mov @stWndClass.hCursor,eax					;---------------------------------------
	push hInstance							;
	pop @stWndClass.hInstance					;
	mov @stWndClass.cbSize,sizeof WNDCLASSEX			;这部分是初始化stWndClass结构中各字段的值，即窗口的各种属性
	mov @stWndClass.style,CS_HREDRAW or CS_VREDRAW			;入门的话，这部分直接copy- -。。。为了赶汇编作业，没时间钻研
	mov @stWndClass.lpfnWndProc,offset _ProcWinMain			;
	;上面这条语句其实就是指定了该窗口程序的窗口过程是_ProcWinMain	;
	mov @stWndClass.hbrBackground,COLOR_WINDOW+1			;
	mov @stWndClass.lpszClassName,offset szClassName		;---------------------------------------
	invoke RegisterClassEx,addr @stWndClass  ;注册窗口类，注册前先填写参数WNDCLASSEX结构

	invoke CreateWindowEx,WS_EX_CLIENTEDGE,\  ;建立窗口
			offset szClassName,offset szCaptionMain,\  ;szClassName和szCaptionMain是在常量段中定义的字符串常量
			WS_OVERLAPPEDWINDOW,100,100,600,400,\	;szClassName是建立窗口使用的类名字符串指针，这里是'MyClass'，表示用'MyClass'类来建立这个窗口，这个窗口拥有'MyClass'的所有属性
			NULL,NULL,hInstance,NULL		;如果改成'button'那么建立的将是一个按钮，szCaptionMain代表的则是窗口的名称，该名称会显示在标题栏中
	mov hWinMain,eax  ;建立窗口后句柄会放在eax中，现在把句柄放在hWinMain中。
	invoke ShowWindow,hWinMain,SW_SHOWNORMAL  ;显示窗口，注意到这个函数传递的参数是窗口的句柄，正如前面所说的，通过句柄可以找到它所标识的事物
	invoke UpdateWindow,hWinMain  ;刷新窗口客户区

	.while TRUE  ;进入无限的消息获取和处理的循环
		invoke GetMessage,addr @stMsg,NULL,0,0  ;从消息队列中取出第一个消息，放在stMsg结构中
		.break .if eax==0  ;如果是退出消息，eax将会置成0，退出循环
		invoke TranslateMessage,addr @stMsg  ;这是把基于键盘扫描码的按键信息转换成对应的ASCII码，如果消息不是通过键盘输入的，这步将跳过
		invoke DispatchMessage,addr @stMsg  ;这条语句的作用是找到该窗口程序的窗口过程，通过该窗口过程来处理消息
	.endw
	ret
_WinMain endp

main proc
	call _WinMain  ;主程序就调用了窗口程序和结束程序两个函数
	invoke ExitProcess,NULL
	ret
main endp
end main
