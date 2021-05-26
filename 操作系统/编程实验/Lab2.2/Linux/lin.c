#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

//定义缓冲区
struct buffer
{
	int s[3];
	int head;
	int tail;
	int is_empty;
};

//显示缓冲区数据
void showdata(struct buffer *a)
{
	printf("Current Data:");
	for (int i = a->head;;)
	{
		printf("%d ", a->s[i]);
		i++;
		i %= 3;
		if (i == a->tail)
		{
			printf("\n\n");
			return;
		}
	}
}

//P操作
void P(int semid, int n)
{
	struct sembuf temp;
	temp.sem_num = n; //索引
	temp.sem_op = -1; //操作值
	temp.sem_flg = 0; //访问标志
	semop(semid, &temp, 1);
}

//V操作
void V(int semid, int n)
{
	struct sembuf temp;
	temp.sem_num = n; //索引
	temp.sem_op = 1;  //操作值
	temp.sem_flg = 0; //访问标志
	semop(semid, &temp, 1);
}

//获取一个n位的随机数
int getrandomnumber(int n)
{
	srand((unsigned)time(NULL)); //初始化随机数发生器
	int temp = 1;
	while (n--)
		temp *= 10;
	int rn = rand();
	return rn % temp;
}

int main(int argc, char *argv[])
{
	int num = 0;
	int semid = semget(6666, 3, IPC_CREAT); //创建信号量
	if (semid < 0)
	{
		printf("semget error!\n");
		exit(0);
	}
	semctl(semid, 0, SETVAL, 3); //empty信号量初值为3
	semctl(semid, 1, SETVAL, 0); //full信号量初值为0
	semctl(semid, 2, SETVAL, 1); //mutex信号量初值为1

	int shmid = shmget(8888, sizeof(struct buffer), IPC_CREAT); //申请共享内存区
	if (shmid < 0)
	{
		printf("shmget error!\n");
		exit(0);
	}
	struct buffer *addr = shmat(shmid, 0, 0); //将共享段附加到申请通信的进程空间
	if (addr == (void *)-1)
	{
		printf("shmat error!\n");
		exit(0);
	}
	addr->head = 0;
	addr->tail = 0;
	addr->is_empty = 1;

	for (int i = 0; i < 2; i++) //生产者
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			printf("producer fork error!\n");
			exit(0);
		}
		if (pid == 0) //创建生产者
		{
			addr = shmat(shmid, 0, 0); // 把共享内存区对象映射到调用进程的地址空间
			if (addr == (void *)-1)
			{
				printf("producer shmat error!\n");
				exit(0);
			}
			for (int j = 0; j < 6; j++)
			{
				sleep(getrandomnumber(1));
				P(semid, 0); //申请empty
				P(semid, 2); //申请mutex
				num = getrandomnumber(3);
				addr->s[addr->tail] = num;
				addr->tail = (addr->tail + 1) % 3;
				addr->is_empty = 0;

				time_t t;
				time(&t);
				printf("Time: %s", ctime(&t));
				printf("Producer %d putting %d\n", i, num);
				showdata(addr);
				V(semid, 1); //释放full
				V(semid, 2); //释放mutex
			}
			shmdt(addr); //将共享段与子进程解除连接
			exit(0);
		}
	}

	for (int i = 0; i < 3; i++) //消费者
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			printf("consumer fork error!\n");
			exit(0);
		}
		if (pid == 0) //创建消费者
		{
			addr = shmat(shmid, 0, 0);
			if (addr == (void *)-1)
			{
				printf("consumer shmat error!\n");
				exit(0);
			}
			for (int j = 0; j < 4; j++)
			{
				//	srand((unsigned)(time(NULL)+getpid()));//初始化随机数发生器
				sleep(getrandomnumber(1));
				P(semid, 1); //申请full
				P(semid, 2); //申请mutex
				num = addr->s[addr->head];
				addr->head = (addr->head + 1) % 3;
				if (addr->head == addr->tail) //头尾相等时缓冲为空
					addr->is_empty = 1;
				else
					addr->is_empty = 0;

				time_t t;
				time(&t);
				printf("Time: %s", ctime(&t));
				printf("Consumer %d removing %d\n", i, num);
				if (addr->is_empty == 0)
					showdata(addr);
				else
					printf("Empty!\n\n");
				V(semid, 0); //释放empty
				V(semid, 2); //释放mutex
			}
			shmdt(addr); //将共享段与子进程解除连接
			exit(0);
		}
	}

	while (wait(0) != -1)
		;
	shmdt(addr);
	semctl(semid, IPC_RMID, 0); //删除信号量
	shmctl(shmid, IPC_RMID, 0); //删除共享段
	return 0;
}
