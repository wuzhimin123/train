#include "data_global.h"

struct msg msgbuf;
struct shm_addr *shm_buf;
struct env_info_client_addr sm_all_env_info_RT;

void *pthread_client_request(void *arg)
{
    if((msg_key = ftok("/tmp",'g')) < 0)
    {
        perror("ftok failed!\n");
        exit(-1);
    }
    //创建消息队列,key为消息队列的标识符,若队列已存在则报错,访问权限为0666；
    msgid = msgget(msg_key,IPC_CREAT|IPC_EXCL|0666);
    if(msgid == -1)
    {
        //如果消息队列已经存在，则以777权限打开
        if(errno == EEXIST)
            msgid = msgget(msg_key,0777);
        else
        {
            perror("failed to msgget\n");
            exit(-1);
        }
    }
    /*打开串口节点*/
    int st_fd = open("/dev/ttymxc2",O_RDWR|O_NOCTTY|O_NDELAY);
    if(st_fd < 0)
    {
        printf("open usb failed\n");
        exit(-1);
    }
    /*设置串口*/
    set_com_config(st_fd,115200,8,'N',1);


    printf("pthread_client_request\n");

    while(1)
    {
        bzero(&msgbuf,sizeof(msgbuf));//清空消息队列
        printf("wait for client request...\n");
        //接收消息队列的消息，接收长度减去long的长度，因为该结构体第一个成员为long，1L代表消息类型为1，L代表long类型，0代表获取不到则线程阻塞
        msgrcv(msgid,&msgbuf,sizeof(msgbuf) - sizeof(long),1L,0);
        printf ("Get %ldL msg\n", msgbuf.msgtype);
		printf ("text[0] = %#x\n", msgbuf.text[0]);

        /*根据消息类型操作不同线程*/
        switch(msgbuf.msgtype)
        {
            case 1L:
                printf("led\n");
                pthread_mutex_lock(&mutex_led);
                cmd_led = msgbuf.text[0];
                pthread_mutex_unlock(&mutex_led);
                pthread_cond_signal(&cond_led);//唤醒cond_wait的线程
                break;
            case 2L:
                printf("beep\n");
                pthread_mutex_lock(&mutex_buzzer);
                cmd_beep = msgbuf.text[0];
                pthread_mutex_unlock(&mutex_buzzer);
                pthread_cond_signal(&cond_buzzer);
                break;
            case 3L:
                printf("seg\n");
                break;
            case 4L:
                printf("fan\n");
                break;
            case 5L:
                printf("set env data\n");
                break;
            default:
                break;
        }
    }
}

void *pthread_led(void *arg)
{
    printf("pthread_led\n");
    while(1)
    {
        pthread_mutex_lock(&mutex_led);
        pthread_cond_wait(&cond_led,&mutex_led);//条件变量使得在条件不满足时进入休眠，防止一直循环访问。
                                                           //比如若cmd_led还没有被写入，可以进入休眠，并释放锁，等cmd_led写入完成后，唤醒该线程再获取锁
        write(st_fd,&cmd_led,1);
        printf("led pthread is running...cmd_led:\n");
        printf ("cmd_led = %#x\n", cmd_led);
        pthread_mutex_unlock(&mutex_led);
    }
}

void *pthread_buzzer(void *arg)
{
    printf("pthread_buzzer\n");
    while(1)
    {
        pthread_mutex_lock(&mutex_buzzer);
        pthread_cond_wait(&cond_buzzer,&mutex_buzzer);
        write(st_fd,&cmd_beep,1);
        printf("buzzer pthread is running...\n");
        pthread_mutex_unlock(&mutex_buzzer);
    }
}

void *pthread_refresh(void *arg)
{
    /*信号量初始化*/
    if((sem_key = ftok("/tmp", 'i')) < 0)
    {
        perror("sem ftok failed\n");
        exit(-1);
    }

    semid = semget(sem_key,1,IPC_CREAT|IPC_EXCL|0666);//创建信号量
    if(semid == -1)
    {
        if(errno == EEXIST)
            semid = semget(sem_key,1,0777);
        else
        {
            perror("failed to semget\n");
            exit(-1);
        }
    }
    else
        init_sem(semid,0,1);//初始化信号量

    /*共享内存初始化*/
    if((shm_key = ftok("/tmp",'i'))<0)
    {
        perror("shm ftok failed\n");
        exit(-1);
    }

    shmid = shmget(shm_key,N,IPC_CREAT|IPC_EXCL|0666);
    if(shmid == -1)
    {
        if(errno == EEXIST)
            shmid = shmget(shm_key,N,0777);
        else{
            perror("failed to shmget\n");
            exit(-1);
        }
    }
    
    if((shm_buf = (struct addr*)shmat(shmid,NULL,0)) == (void*)-1)//分配的共享内存地址指向struct addr结构体
    {
        perror("failed to shmat\n");
        exit(-1);
    }

    printf("pthread_refreseh ....>>>>>>\n");


    bzero(shm_buf,sizeof(struct shm_addr));
    while(1)
    {
        
        sem_p(semid,0);
        shm_buf->shm_status = 1;
        // file_env_info_struct(&shm_buf->sm_all_env_info,shm_buf->shm_status);
        shm_buf->sm_all_env_info = sm_all_env_info_RT;
        sleep(1);
        sem_v(semid,0);

    }

}

void *pthread_transfer(void *arg)
{

	printf("pthread_transfer\n");

    /*打开串口节点*/
    int st_fd = open("/dev/ttymxc2",O_RDWR|O_NOCTTY|O_NDELAY);
    if(st_fd < 0)
    {
        printf("open usb failed\n");
        exit(-1);
    }
    /*设置串口*/
    set_com_config(st_fd,115200,8,'N',1);

    char buf[sizeof(struct storage_info)]={0};
    printf("sizeof(buf) = %d.\n",sizeof(buf));

    while(1)
    {
        memset(buf,0,sizeof(buf));
        recvDats(st_fd,&buf[0],1);
        if(buf[0]!=0x55)//起始帧检查
            continue;
        recvDats(st_fd,&buf[0],sizeof(buf));//读数据
        memcpy(&(sm_all_env_info_RT.storage_no[1]),buf,sizeof(buf));
        
        if(sm_all_env_info_RT.storage_no[1].t >= TEMPERATURE_THRESHOLD || sm_all_env_info_RT.storage_no[1].curren >= CURRENT_THRESHOLD);
        {
            // write(st_fd,0x61,1);//打开led
            // sleep(100);
            // write(st_fd,0x51,1);//蜂鸣器报警
            // sleep(100);

            /*打开imx6ull beep*/
            int fd = open("/dev/beep",O_RDWR);
            if(fd < 0)
            {
                printf("open imx6ull beep failed\n");
                exit(-1);
            }
            unsigned char databuf[1];
            databuf[0] = 0x01;
            int retvalue = write(fd,databuf,1);
            if(retvalue < 0)
            {
                printf("write imx6ull beep failed\n");
                close(fd);
                exit(-1);
            }
            close(fd);

            /*LED打开命令*/ 
            pthread_mutex_lock(&mutex_led);
            cmd_led = 0x61;
            pthread_mutex_unlock(&mutex_led);
            pthread_cond_signal(&cond_led);//唤醒cond_wait的线程

            /*stm32 beep打开命令*/
            pthread_mutex_lock(&mutex_buzzer);
            cmd_beep = 0x51;
            pthread_mutex_unlock(&mutex_buzzer);
            pthread_cond_signal(&cond_buzzer);//唤醒cond_wait的线程

        }
        sleep(1);
    }
    close(st_fd);


}