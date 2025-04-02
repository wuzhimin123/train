
#include "sem.h"


int semid;

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

int init_sem(int semid, int num, int val)
{
    union semun initsem;
    initsem.val = val;
    /*初始化信号量的值为0*/
    if(semctl(semid,num,SETVAL,initsem)<0)
    {
        perror("semctl error!\n");
        exit(1);
    }
    return 0;
}

/*p操作*/
int sem_p(int semid, int num)
{
    struct sembuf set;
    set.sem_num = 0;//第0号信号量
    set.sem_op = -1;//信号量-1操作
    set.sem_flg = SEM_UNDO;
    /*对信号量id进行操作，1代表只对信号量集中的一个信号量操作*/
    if(semop(semid,&set,1) < 0)
    {
        perror("semop error!\n");
        exit(1);
    }
    return 0;
}

/*v操作*/
int sem_v(int semid, int num)
{
    struct sembuf set;
    set.sem_num = 0;//第0号信号量
    set.sem_op = 1;//信号量+1操作
    set.sem_flg = SEM_UNDO;
    /*对信号量id进行操作，1代表只对信号量集中的一个信号量操作*/
    if(semop(semid,&set,1) < 0)
    {
        perror("semop error!\n");
        exit(1);
    }
    return 0;
}

