#ifndef __SEM_H__
#define __SEM_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

extern int semid;

extern int init_sem(int semid, int num, int val);

/*p操作*/
extern int sem_p(int semid, int num);

/*v操作*/
extern int sem_v(int semid, int num);


#endif