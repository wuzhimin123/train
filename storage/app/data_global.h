#ifndef __DATA_GLOBAL_H__
#define __DATA_GLOBAL_H__
/*防止一个源文件中重复包含该头文件*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include "uart_app.h"
#include "sem.h"

#define QUEUE_MSG_LEN 32
#define N 1024
#define MONITOR_NUM 1
#define TEMPERATURE_THRESHOLD 30.0f
#define CURRENT_THRESHOLD 50.0f
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

key_t msg_key;
key_t shm_key;
key_t sem_key;

uint8_t cmd_led;
uint8_t cmd_beep;

int st_fd;
int shmid;
int msgid;


extern void *pthread_client_request (void *arg); //接收CGI 等的请求
extern void *pthread_refresh(void *arg);              //刷新共享内存数据线程
extern void *pthread_sqlite(void *arg);                 //数据库线程，保存数据库的数据
extern void *pthread_transfer(void *arg);           //接收ZigBee的数据并解析
extern void *pthread_buzzer(void *arg);          //蜂鸣器控制线程
extern void *pthread_led(void *arg);                 //led灯控制线程



extern pthread_mutex_t mutex_client_request,
                mutex_buzzer,
                mutex_led;

/*条件变量，用于唤醒互斥锁导致休眠的线程*/
extern pthread_cond_t cond_client_request,
                cond_buzzer,
                cond_led;

extern pthread_t id_client_request,
                id_refresh,
                id_sqlite,
                id_transfer,
                id_sms,
                id_buzzer,
                id_led;
/*消息队列结构体*/
struct msg
{
    long type;//消息类型
    long msgtype;//具体的消息类型
    uint8_t text[QUEUE_MSG_LEN];//内容
};

struct storage_info
{
	unsigned char storage_status;				// 0:open 1:close
	uint16_t gyrox;   //陀螺仪数据
	uint16_t gyroy;
	uint16_t gyroz;
	uint16_t  aacx;  //加速计数据
	uint16_t  aacy;
	uint16_t aacz;
	float t;		//温度
    float curren;//电流数据
};

struct env_info_client_addr
{
	struct storage_info storage_no[MONITOR_NUM];	
};

// //所有监控区域的信息结构体
// struct env_info_client_addr
// {
// 	struct makeru_env_data  monitor_no[MONITOR_NUM];	//数组  老家---新家
// };

// extern int file_env_info_struct(struct env_info_client_addr *rt_status,int home_id);

struct shm_addr
{
    char shm_status;
    struct env_info_client_addr sm_all_env_info;
};



#endif

