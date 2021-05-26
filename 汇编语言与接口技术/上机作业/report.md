## 摘要

汇编大作业中，我选择上机题目2，3，5，进行汇编代码实现。第二个实验，大数相乘，参考第五个实验的方法，类比C语言编写程序的代码思路，实现的关键在于逆序存放数组，以及先大数乘法，再统一进位；第三个实验，我初步了解到汇编语言如何实现一个窗口程序。其中，窗口过程函数，响应窗口的各个部件的变化，可类比面向对象编程的回调函数；第五个实验，多重循环程序，显示了循环的实现，本质上为数据流的方向变更。

关键字：大数相乘；文件内容比较；多重循环

## Abstract

In the assembly assignment, I have chose the computer problems 2, 3 as well as 5 to implement the assembly code. The second experiment, multiplying large numbers, refers to the method of the fifth experiment, analogous to the C language to write a program code, the key to implementation is to store the array in reverse order, and multiply the large number first, and then carry together; the third experiment, I initially learned how assembly language implements a window program. Among them, the window procedure function, in response to the changes of the various components of the window, can be compared to the callback function of object-oriented programming; the fifth experiment, the multi-loop program, shows the implementation of the loop, which is essentially the direction change of the data flow.

keywords： the multiplying large numbers program; the file content comparison program; the multi-loop program

## 第一章 大数相乘

### 1.1 实验目的

​	大数相乘。要求实现两个十进制大整数的相乘（100位以上），输出乘法运算的结果。

### 1.2 实验步骤

1. 确定算法思路
2. 编写汇编代码
3. 运行汇编代码

### 1.3 实验环境

1. Microsoft Visual Studio Community 2019
2. 汇编语言: masm 32

### 1.4 代码思路

​	本代码内容参考了C语言编写的大数相乘程序的思路，下面结合C语言程序进行思路阐述：

#### 1.4.1输入数组

​	调用库函数scanf获取输入字符串(大数)，库函数strlen获取大数的位数。参考C语言代码和汇编代码见下：

```c
char snum1[200] = { '\0' };
char snum2[200] = { '\0' };
scanf("%s", snum1);
scanf("%s", snum2);
const int len1 = strlen(snum1);
const int len2 = strlen(snum2);

```

```assembly
L0:
            INVOKE	scanf,	offset string_in,	OFFSET snum1      
            INVOKE	strlen,	offset snum1					
            MOV		len1,	eax
            INVOKE	scanf,	offset string_in,	offset snum2
            INVOKE	strlen,	offset snum2                     
            MOV		len2,	eax
```

#### 1.4.2倒序存储数组

​	将字符串数组每位减去'0'，转为整型数组。为方便计算，这里选择倒叙存储计算的大数。

```c
int num1[200] = { 0 };

for (int i = 0, j = len1 - 1; i < len1; i++, j--) {
    num1[i] = snum1[j] - '0';
}
```

```assembly
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
```

#### 1.4.3 大数相乘

​	先不考虑进位，进行每位的乘法运算。其中，结果数组的第i+j位的值等于所有大数1的第i位和大数2的第j位的乘法结果之和：

```c
int ans[210] = { 0 };
for (int i = 0; i < len1; i++) {
    for (int j = 0; j < len2; j++) {
        ans[i + j] += num1[i] * num2[j];
    }
}
```

```assembly
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
```

#### 1.4.4 进位处理

​	首先，未处理进位的结果数组的长度的值，是大数1和大数2的位数和减2；然后，进行进位处理：结果数组的当前位的值除以10，商即为进位，余数即为当前位的更新值，最后需要根据最后一位是否有进位，调整结果数组长度。

```c
int len = len1 + len2 - 2;
for (int i = 0; i <= len; i++) {
    int carry = ans[i] / 10;
    ans[i] %= 10;
    ans[i + 1] += carry;

    if (i == len && carry != 0) {
        len++;
    }
}
```

```assembly
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
```



### 1.5 运行结果

​	点击运行按钮，输入数字9999999999999999999，9999999999999999999。输出运算结果图见下：

<img src="C:\study\汇编语言与接口技术\code\picture\图1.5-1.jpg" style="zoom:50%;" />

​															图1.5-1 运行结果

## 第二章 文本文件内容

### 2.1 实验目的

​	Windows界面风格实现两个文本文件内容的比对。若两文件内容一样，输出相应提示；若两文件不一样，输出对应的行号

### 2.2 实验步骤

1. 确定算法思路
2. 编写汇编代码
3. 运行汇编代码

### 2.3 实验环境

1. Microsoft Visual Studio Community 2019
2. 汇编语言: masm 32

### 2.4 代码思路

#### 2.4.1 win32 窗口程序初始化

##### 2.4.1.1 初始化窗口

​	初始化窗口，绑定窗口过程函数：

```assembly
MOV		structWndClass.hCursor,		eax
push	hInstance
pop		structWndClass.hInstance
MOV		structWndClass.cbSize,	sizeof	WNDCLASSEX
MOV		structWndClass.style,	CS_HREDRAW	OR	CS_VREDRAW
;指定该窗口程序的窗口过程是_ProcWinMain
MOV		structWndClass.lpfnWndProc,	offset	_ProcWinMain	
MOV		structWndClass.hbrBackground,	COLOR_WINDOW + 1
MOV		structWndClass.lpszClassName,	offset	winClassName

;先填写WNDCLASSEX的结构再注册
INVOKE	RegisterClassEx,	addr		structWndClass		
```

##### 2.4.1.2 显示窗口

​	显示初始化后的窗口并刷新窗口客户区：

```assembly
INVOKE	ShowWindow,	hWinMain,	SW_SHOWNORMAL	;显示窗口
INVOKE	UpdateWindow,		hWinMain		;刷新窗口客户区
```

##### 2.4.1.3 无限循环的消息获取和处理

​	进入无限循环的消息获取和处理，窗口过程处理消息。当eax为0时，退出循环

```assembly
.WHILE TRUE    ;进入无限循环的消息获取和处理
	INVOKE	GetMessage,	addr	structMsg,	NULL,	0,	0
	.break	.IF		eax==0   ;若退出循环，eax置为零
	
	;将基于键盘扫描码的按键信息转换成对应的ASCII码，若消息非键盘输入，这步跳过
	INVOKE	TranslateMessage,	addr	structMsg		
	;通过该窗口的窗口过程处理消息
	INVOKE	DispatchMessage,	addr	structMsg		
.ENDW
```

#### 2.4.2 win32窗口过程函数

##### 2.4.2.1 创建窗口

​	创建窗口，窗口中有两个文本框，一个按钮，其中按钮的ID为3。

```assembly
.ELSEIF	eax==WM_CREATE    ;创建窗口
	;第一个文本框
	INVOKE	CreateWindowEx,	WS_EX_CLIENTEDGE, \
			OFFSET edit, NULL,WS_CHILD OR  WS_VISIBLE	OR \			WS_BORDER OR	ES_LEFT	OR	ES_AUTOHSCROLL,\
			10, 10, 350, 35,hWnd,	1,	hInstance,	NULL   
	MOV		hwndEdit1,		eax									
	;第二个文本框
	INVOKE	CreateWindowEx,	WS_EX_CLIENTEDGE, OFFSET edit, \			NULL,WS_CHILD OR  WS_VISIBLE	OR WS_BORDER\ 				OR	ES_LEFT	OR	ES_AUTOHSCROLL,\
			10, 50, 350, 35, hWnd,	2,		hInstance,	NULL 
	MOV		hwndEdit2,		eax		
	
	;确认按键
	INVOKE	CreateWindowEx,	NULL, OFFSET button, OFFSET \				showButton, WS_CHILD OR  WS_VISIBLE,\
    		220, 100, 200, 45,hWnd,	3,		hInstance,	NULL    
```

##### 2.4.2.2 响应确认按键

​	响应按钮的点击(eax == 3)，调用GetWindowText函数获取文本框内容，即两个文件的路径名；然后调用函数_CompareFile，进行文件内容比较；最后，当不同行数总数为零时，显示窗口内容为"there is no different _line between file1 and file2"，否则，显示窗口为不同行数。

```assembly

.ELSEIF	eax==WM_COMMAND
	mov eax,wParam  
	;其中参数wParam里存的是句柄，如果点击了一个按钮，则wParam是那个按钮的句柄
	.IF	eax ==3
		invoke GetWindowText,hwndEdit1,ADDR filePath1,512
		invoke GetWindowText,hwndEdit2,ADDR filePath2,512
		INVOKE	_CompareFile,	offset	filePath1,	\
				OFFSET filePath2
		.IF	diffNum == 0
			INVOKE	MessageBox,	hWnd,	OFFSET SameContent,\				OFFSET szBoxTitle,	MB_OK + MB_ICONQUESTION
		.ELSE
			;输出不同的行数
			INVOKE		MessageBox,	hWnd,	OFFSET diffOut,\				OFFSET szBoxTitle,	MB_OK + MB_ICONQUESTION
		.ENDIF
	.ENDIF
```

#### 2.4.3读取文件

​	此处参考C语言的程序思路，首先打开两个文件；然后分别读取一行内容，比较一行内容是否相同，如果不同，则记录在数组中；最后，当两个文件读写指针都到达文件末尾时，结束读取文件。

##### 2.4.3.1 打开文件

​	此处通过输入文件路径名，调用函数CreateFile，获取文件句柄

```assembly
INVOKE	CreateFile, lpFilePath1, GENERIC_READ,\ 					FILE_SHARE_READ, NULL,OPEN_EXISTING, \
		FILE_ATTRIBUTE_NORMAL, NULL
MOV		fp1,	eax

INVOKE	CreateFile, lpFilePath2, GENERIC_READ,\ 					FILE_SHARE_READ, NULL,OPEN_EXISTING,\
 		FILE_ATTRIBUTE_NORMAL, NULL
MOV		fp2,	eax
```

##### 2.4.3.2 读取文件中的一行

​	由于ReadFile函数，只通过指定字符数量读取文件内容，无法一次性读取文件中一行。所以，编写了一个新函数__ReadLine，读取文件中的一行。具体思路是，调用ReadFile函数，一次只读一个字符，当读取字符为普通字符时，存入返回数组，当读取字符为换行符或为空字符时，退出循环。

```assembly
_ReadLine	PROC	USES	ebx, fp:HANDLE,	buff:ptr byte
    LOCAL	lpNum:	dword
    LOCAL	_str:	BYTE

    MOV		ebx,	buff
    .while	TRUE
		INVOKE	ReadFile,	fp,	addr _str,	1,\
			addr lpNum,	NULL
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
```

##### 2.4.3.3 比较两行内容

​	比较两行文本内容，思路可用2.4.3.3-1的流程图解释。

<img src="C:\study\汇编语言与接口技术\code\picture\fig_2_4_3_3_1.png" style="zoom:50%;" />

​															图2.4.3.3 比较文本内容

​	一共可分为四种情况，文件1和文件2读指针都到文件末尾，这时退出循环；文件1读指针已到文件末尾，文件2读指针未到文件末尾，这时应记录行数；文件2读指针已到文件末尾，文件1读指针未到文件末尾，这时应记录行数；文件1和文件2读指针都未到文件末尾，这时需要进行字符串比较，调用lstrcmp函数，当返回值为0时，该行相同，否则记录下行数，见下述代码。

```assembly
INVOKE	lstrcmp,	OFFSET	buff1,	OFFSET	buff2
CMP		eax,	0
JE		CYCLE

INVOKE	sprintf,	ADDR diffTem,	OFFSET	DiffContent,\			__line
INVOKE	lstrcat,	ADDR diffOut,	ADDR	diffTem
INC		diffNum
```



### 2.5 运行结果

​	点击运行按钮，在第一文本框中输入第一个文件的路径'test1.txt'，第二个文本框中输入第二个文件的路径'test2.txt'，点击SURE按钮：

<img src="C:\study\汇编语言与接口技术\code\picture\fig_2_5_1.jpg" style="zoom:50%;" />

​															图2.5-1 输入1

​	运行结果见下：

<img src="C:\study\汇编语言与接口技术\code\picture\fig_2_5_2.jpg" style="zoom:50%;" />

​																图2.5-2 运行结果1

​	点击运行按钮，在第一文本框中输入第一个文件的路径'test1.txt'，第二个文本框中输入第二个文件的路径'test1.txt'，点击SURE按钮：

<img src="C:\study\汇编语言与接口技术\code\picture\fig_2_5_3.jpg" style="zoom:50%;" />

​																图2.5-3 运行结果2

运行结果见下：

<img src="C:\study\汇编语言与接口技术\code\picture\fig_2_5_4.jpg" style="zoom:50%;" />

​																图2.5-4 运行结果2

## 第三章 多重循环程序

### 3.1 实验目的

​	C语言编写多重循环程序（大于3重），查看其反汇编码，分析各条语句功能（分析情况需要写入实验报告），并采用汇编语言重写相同功能程序

### 3.2实验步骤

1. 编写C语言多重循环程序(大于三重)
2. 查看并分析其反汇编代码
3. 采用汇编语言重写相同程序

### 3.3 实验环境

1. Microsoft Visual Studio Community 2019
2. 汇编语言: masm 32

### 3.4 具体实验内容

#### 3.4.1 C语言多重循环程序

​	程序主要功能是求解一个给定值的四元二次方程，将所有可能的情况打印出来，当无解时，输出"No solution"。其代码实现见下:

```c
#include <stdio.h>
int main() {
	int N = 2;
	bool flag = false;
	for (int i = 0; i * i < N; i++) {
		for (int j = 0; j * j < N; j++) {
			for (int z = 0; z * z < N; z++) {
				for (int y = 0; y * y < N; y++) {
					int ans = i * i + j * j + z * z + y * y;
					if (ans == N) {
						printf("%d^2 + %d^2  + %d^2 + %d^2 = %d\n", i, j, z, y, N);
						flag = true;
					}
				}
			}
		}
	}
	if (!flag) {
		printf("No solution.\n");
	}
}
```

#### 3.4.2 查看并分析其反汇编程序

​	进入调试页面后，点击工具栏中的调试->窗口->反汇编，查看反汇编源码。

##### 3.4.2.1 函数调入

​	函数调用的汇编代码见下：

```assembly
;int main() {
005C2A80  push        ebp  
005C2A81  mov         ebp,esp  
005C2A83  sub         esp,114h  
005C2A89  push        ebx  
005C2A8A  push        esi  
005C2A8B  push        edi  
005C2A8C  lea         edi,[ebp-114h]  
005C2A92  mov         ecx,45h  
005C2A97  mov         eax,0CCCCCCCCh  
005C2A9C  rep stos    dword ptr es:[edi]  
005C2A9E  mov         ecx,offset _BAB9F34C_main@cpp (05CD003h)  
005C2AA3  call        @__CheckForDebuggerJustMyCode@4 (05C1217h) 
```

1. `005C2A80  push        ebp  `：将前栈底指针压入栈中
2. `005C2A81  mov         ebp,esp`：更新ebp值，ebp存入当前esp的值
3. `005C2A83  sub         esp,114h`：更新esp值，esp - 114H，预留出0x114h个单位的地址空间存局部变量
4. `005C2A89  push        ebx  `：保留原始内存偏移指针
5.  `005C2A8A  push        esi `：保留源地址指针
6. `005C2A8B  push        edi`：保留目的地址指针
7. `005C2A8C  lea         edi,[ebp-114h] `：edi存取栈顶指针
8. `  005C2A9C rep stos    dword ptr es:[edi]  `：rep指示重复`005C2A92  mov         ecx,45h  `，`005C2A97  mov         eax,0CCCCCCCCh `两条指令，ecx中存有重复次数0x45h，将栈中从[ebp-114H]开始位置向高地址方向赋值0xCCCCCCCCh，重复0x45h次。stops指令的作用是将eax中的值拷贝到es:[edi]指向的位置

##### 3.4.2.2局部变量初始化

​	函数开始部分有两个局部变量的初始化: N和flag，格式为"mov 	类型 ptr [变量名], 初始值"。具体内容见下：

```assembly
	;int N = 2;
005C2AA8  mov         dword ptr [N],2  
	;bool flag = false;
005C2AAF  mov         byte ptr [flag],0  
```

##### 3.4.2.3 循环体

​	下面展示了循环体的具体反汇编代码，用 ';----------------' 将代码分块。

​	下面这四块代码块展示了四个for循环。以第一个for循环为例进行说明，不妨设第一个for循环代码为第一组代码块，代码块从0开始标号，一共三个代码块。第0个代码块 `005C2AB3 - 005C2ABA`，表示给双字型变量 i 赋值为0，无条件跳转到第2个代码块；第1个代码块 `005C2ABC - 005C2AC2`，表示变量 i 自增1；第2个代码块 `005C2AC5 - 005C2ACF`，表示变量 i*i 的结构与变量N比较大小，若 i * i \>=  N，则跳转至 0x5C2B96h。其流程可以将其抽象为图3.4.2.3-1。

```assembly
;for (int i = 0; i * i < N; i++) {
005C2AB3  mov         dword ptr [ebp-20h],0  
005C2ABA  jmp         main+45h (05C2AC5h) 
;----------------
005C2ABC  mov         eax,dword ptr [ebp-20h]  
005C2ABF  add         eax,1  
005C2AC2  mov         dword ptr [ebp-20h],eax  
;----------------
005C2AC5  mov         eax,dword ptr [ebp-20h]  
005C2AC8  imul        eax,dword ptr [ebp-20h]  
005C2ACC  cmp         eax,dword ptr [N]  
005C2ACF  jge         main+116h (05C2B96h)  
;----------------
```

```assembly
;for (int j = 0; j * j < N; j++) {
005C2AD5  mov         dword ptr [ebp-2Ch],0  
005C2ADC  jmp         main+67h (05C2AE7h)  
;----------------
005C2ADE  mov         eax,dword ptr [ebp-2Ch]  
005C2AE1  add         eax,1  
005C2AE4  mov         dword ptr [ebp-2Ch],eax 
;----------------
005C2AE7  mov         eax,dword ptr [ebp-2Ch]  
005C2AEA  imul        eax,dword ptr [ebp-2Ch]  
005C2AEE  cmp         eax,dword ptr [N]  
005C2AF1  jge         main+111h (05C2B91h)  
;----------------
```

```assembly
;for (int z = 0; z * z < N; z++) {
005C2AF7  mov         dword ptr [ebp-38h],0  
005C2AFE  jmp         main+89h (05C2B09h)  
;----------------
005C2B00  mov         eax,dword ptr [ebp-38h]  
005C2B03  add         eax,1  
005C2B06  mov         dword ptr [ebp-38h],eax 
;----------------
005C2B09  mov         eax,dword ptr [ebp-38h]  
005C2B0C  imul        eax,dword ptr [ebp-38h]  
005C2B10  cmp         eax,dword ptr [N]  
005C2B13  jge         main+10Ch (05C2B8Ch)  
;----------------
```

```assembly
;for (int y = 0; y * y < N; y++) {
005C2B15  mov         dword ptr [ebp-44h],0  
005C2B1C  jmp         main+0A7h (05C2B27h)
;----------------
005C2B1E  mov         eax,dword ptr [ebp-44h]  
005C2B21  add         eax,1  
005C2B24  mov         dword ptr [ebp-44h],eax  
;----------------
005C2B27  mov         eax,dword ptr [ebp-44h]  
005C2B2A  imul        eax,dword ptr [ebp-44h]  
005C2B2E  cmp         eax,dword ptr [N]  
005C2B31  jge         main+107h (05C2B87h)  
;----------------
```

<img src="C:\study\汇编语言与接口技术\code\picture\图3.4.2.3-1.jpg" style="zoom:50%;" />

​															图3.4.2.3-1 循环体流程

下面四组代码展示了最内层循环代码内容：

首先，求多项式和。主要流程很简单，大致可以概述为：先计算i * i，结果保留在eax；再计算j * j，结果保留在ecx；然后，将eax中的值与ecx中值进行相加，结果保留在eax； 紧接着，计算 z * z，结果保留在edx；然后，将eax中的值与edx中值进行相加，结果保留在eax；最后，计算y * y，结果保留在ecx，ecx 中的值与eax中值进行相加，eax存放计算结果。

```assembly
;int ans = i * i + j * j + z * z + y * y;
005C2B33  mov         eax,dword ptr [ebp-20h]  
005C2B36  imul        eax,dword ptr [ebp-20h]  
005C2B3A  mov         ecx,dword ptr [ebp-2Ch]  
005C2B3D  imul        ecx,dword ptr [ebp-2Ch]  
005C2B41  add         eax,ecx  
005C2B43  mov         edx,dword ptr [ebp-38h]  
005C2B46  imul        edx,dword ptr [ebp-38h]  
005C2B4A  add         eax,edx  
005C2B4C  mov         ecx,dword ptr [ebp-44h]  
005C2B4F  imul        ecx,dword ptr [ebp-44h]  
005C2B53  add         eax,ecx  
005C2B55  mov         dword ptr [ebp-50h],eax  
```

​	然后，判断ans值是否为N。将ans的值与变量N进行比较，当ans不等于N时，跳转至 `0x5c2B85h`:

```assembly
;if (ans == N) {
005C2B58  mov         eax,dword ptr [ebp-50h]  
005C2B5B  cmp         eax,dword ptr [N]  
005C2B5E  jne         main+105h (05C2B85h)
;----------------
```

​	接着，调用printf函数，打印值。将需打印的变量，逆序压入栈中：N, y, z, j, i；然后，伪指令offset，替换成字符串"%d^2 + %d^2  + %d^2 + %d^2 = %d@"的地址偏移，压入栈中；最后，调用函数printf，然后销毁printf函数调用的参数。

```assembly
;printf("%d^2 + %d^2  + %d^2 + %d^2 = %d\n", i, j, z, y, N);
005C2B60  mov         eax,dword ptr [N]  
005C2B63  push        eax  
005C2B64  mov         ecx,dword ptr [ebp-44h]  
005C2B67  push        ecx  
005C2B68  mov         edx,dword ptr [ebp-38h]  
005C2B6B  push        edx  
005C2B6C  mov         eax,dword ptr [ebp-2Ch]  
005C2B6F  push        eax  
005C2B70  mov         ecx,dword ptr [ebp-20h]  
005C2B73  push        ecx  
005C2B74  push        offset string "%d^2 + %d^2  + %d^2 + %d^2 = %d@"... (05C8B30h)  
005C2B79  call        _printf (05C1046h)  
005C2B7E  add         esp,18h  
```

​	最后，给变量flag赋值。可见赋值代码与初始化的代码类似。

```assembly
;flag = true;
005C2B81  mov         byte ptr [flag],1  
```

​	下面代码展示了循环结束后的跳转：

```assembly
					}
				}
005C2B85  jmp         main+9Eh (05C2B1Eh)
;----------------
			}
005C2B87  jmp         main+80h (05C2B00h)
;----------------
		}
005C2B8C  jmp         main+5Eh (05C2ADEh)  
;----------------
	}
005C2B91  jmp         main+3Ch (05C2ABCh)  
;----------------
```

##### 3.4.2.4 方程无解处理

​	可参考循环体中代码的解析。`005C2B9A  test        eax,eax ` ，对变量flag的值进行逻辑与操作，不改变flag的值，当结果不为0时，zf为0，当结果为0时，zf为1，jne通过检测zf值实现条件跳转，当zf为0时，跳转到 `0x5C2BABh`。

```assembly
;if (!flag) {
005C2B96  movzx       eax,byte ptr [flag]  
005C2B9A  test        eax,eax  
005C2B9C  jne         main+12Bh (05C2BABh) 
;----------------
		;printf("No solution.\n");
005C2B9E  push        offset string "No solution.\n" (05C8B58h)  
005C2BA3  call        _printf (05C1046h)  
005C2BA8  add         esp,4  
	}
}
```

##### 3.4.2.5 函数返回

​	下面展示了函数返回的汇编代码。首先，通过，异或`xor`操作，将 eax清空；然后，恢复原来调用者的edi， esi，ebx；紧接着，栈顶指针esp，回退0x114h个地址单元；然后，将当前ebp值赋给esp；最后恢复原有的ebp。整个流程可以用图3.4.2.5-1进行说明。

```assembly
;----------------
005C2BAB  xor         eax,eax  
005C2BAD  pop         edi  
005C2BAE  pop         esi  
005C2BAF  pop         ebx  
005C2BB0  add         esp,114h  
005C2BB6  cmp         ebp,esp  
005C2BB8  call        __RTC_CheckEsp (05C1221h)  
005C2BBD  mov         esp,ebp  
005C2BBF  pop         ebp  
005C2BC0  ret  
;----------------
```

<img src="C:\study\汇编语言与接口技术\code\picture\图3.4.2.5-1.jpg" style="zoom:50%;" />

​															图3.4.2.5-1 函数返回流程

#### 3.4.3 汇编代码重写多重循环

​	参考C源程序进行编写多重循环。下面展示了重写代码中的循环体部分，其难点在于循环体中的各类跳转。与反汇编代码不同之处在于，使用标准库，函数可直接调用，不需进行压栈操作。

```assembly
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
```

### 3.5	实验结果展示

​	点击运行按钮，输出满足结果为2的四元二元方程的几组可能解：

![](C:\study\汇编语言与接口技术\code\picture\图3.5-1.jpg)

​															图3.5-1 运行结果



## 总结

汇编大作业中，我选择上机题目2，3，5，进行汇编代码实现。这三次实验，使我对汇编语言编程有了更深的了解。第二个实验，大数相乘，参考第五个实验的方法，类比C语言编写程序的代码思路，实现的关键在于逆序存放数组，以及先大数乘法，再统一进位；第三个实验，我初步了解到汇编语言如何实现一个窗口程序。其中，窗口过程函数，响应窗口的各个部件的变化，可类比面向对象编程的回调函数；第五个实验，多重循环程序，显示了循环的实现，本质上为数据流的方向变更。