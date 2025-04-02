#include "data_global.h"

void release_pthread_resource(int signal);

int main(int argc, const char *argv[])
{
    /*初始化互斥锁*/
    pthread_mutex_init(&mutex_client_request,NULL);
    pthread_mutex_init(&mutex_buzzer,NULL);
    pthread_mutex_init(&mutex_led,NULL);

    /*当接收到ctrl+c后，会调用处理函数*/
    signal(SIGINT,release_pthread_resource);

    /*初始化条件变量*/
    pthread_cond_init(&cond_client_request,NULL);
    pthread_cond_init(&cond_buzzer,NULL);
    pthread_cond_init(&cond_led,NULL);

    /*线程创建*/
    pthread_create(&id_client_request,NULL,pthread_client_request,NULL);
    pthread_create(&id_refresh,NULL,pthread_refresh,NULL);
    pthread_create(&id_transfer,NULL,pthread_transfer,NULL);
    pthread_create(&id_buzzer,NULL,pthread_buzzer,NULL);
    pthread_create(&id_led,NULL,pthread_led,NULL);

    /*等待线程结束*/
    pthread_join(id_client_request,NULL);   printf("pthread1\n");
    pthread_join(id_refresh,NULL);           printf("pthread2\n");
	pthread_join(id_transfer,NULL);			printf ("pthread3\n");
	pthread_join(id_buzzer,NULL);			printf ("pthread4\n");
	pthread_join(id_led,NULL);				printf ("pthread5\n");

    return 0;
}

void release_pthread_resource(int signal)
{
    /*释放锁*/
    pthread_mutex_destroy(&mutex_client_request);
    pthread_mutex_destroy(&mutex_buzzer);
    pthread_mutex_destroy(&mutex_led);

    /*释放条件变量*/
    pthread_cond_destroy(&cond_client_request);
 	pthread_cond_destroy (&cond_buzzer);
 	pthread_cond_destroy (&cond_led);

    /*使线程处于分离态，自动回收资源，不需要pthread_join了*/
    pthread_detach(id_client_request);
    pthread_detach(id_refresh);
	pthread_detach(id_transfer);
	pthread_detach(id_buzzer);
	pthread_detach(id_led);

    printf("all pthread is detached\n");

    /*删除消息队列、共享内存、信号量*/
    msgctl (msgid, IPC_RMID, NULL);
	shmctl (shmid, IPC_RMID, NULL);
	semctl (semid, 1, IPC_RMID, NULL);

    exit(0);
}