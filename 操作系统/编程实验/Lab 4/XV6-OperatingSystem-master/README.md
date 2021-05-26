Operating System (XV6)
===
CONTENT:
---
These files contain the work I have done to the XV6 operating system to practice designing O.S.

ADDED FUNCTIONALITIES:
---
1. I have created new system call called "exit" which stores an exit status once a process is terminated. This exit call returns 0 when the process terminates succesfully and -1 when there has been an error.
2. I have created a "waitpid" system call which is a blocking waitpid where the kernel prevents the current process from execution until a process with the given pid terminates. The waitpid returns the pid of the process that was terminated or a -1 when the process does not exit or if an unexpected error occured.
3. Implementing these two system calls required modifications to many pre-existing functions to support the new exit status.

ADDED FUNCTIONALITIES:
---
I implemented a priority scheduler by modifying the pre-existing round-robin scheduler. Each process has a priority value ranging from 31-0 with 0 being the highest priority. A process is able to change priority at anytime. I have avoided starvation by implementing aging priority, where each unfinished process increases in priority once the scheduler has iterated through the scheduling table once. I have also implemented priority inheritence, where a child process will have the same priority as the parent process after forking.

ADDED FUNCTIONALITIES:
---
I have changed the user memory layout of XV6 to allow a heap to grow towards the high-end of the address space and to allow the stack to grow backwards since XV6 did not allow the stack to grow. Initially, the memory layout was order code->stack->heap. I have rearranged the layout to emulate linux; code->heap->stack which gives room for the stack to grow. I utilized the trap "T_PGFLT" to cause a kernel panic which then allowed me to detect the need to grow the stack by a single page. This required me to create a traphandler, allocuvm, and mappages system call. 

ADDED FUNCTIONALITIES:
---
I have implemented support to enable two processes to share a memory page. I have created a new system call: shm_open, which allows a program to fork and then have both processes open a shared memory segment. Shm_open utilizes a pointer to the shared memory address so we are able to access this shared memory page. This allows threading to be implemented. I have had to create a user level spin lock to prevent race-conditions by implementing the function: uspinlock. 

LANQUAGE:
---
C
