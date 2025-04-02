#include "data_global.h"

pthread_mutex_t mutex_client_request,
                mutex_buzzer,               
                mutex_led;

/*条件变量，用于唤醒互斥锁导致休眠的线程*/
pthread_cond_t cond_client_request,
                cond_buzzer,
                cond_led;
                
/*线程id*/
pthread_t id_client_request,
            id_refresh,
            id_sqlite,
            id_transfer,
            id_buzzer,
            id_led;


// int file_env_info_struct(struct env_info_client_addr *rt_status,int home_id)
// {
// 	int  env_info_size = sizeof(struct env_info_client_addr);
// 	//printf("env_info_size = %d.\n",env_info_size);
//     rt_status->storage_no[home_id].storage_status = 1;     
// 	rt_status->storage_no[home_id].t = 20.0;
// 	rt_status->storage_no[home_id].gyrox= 2.0;
// 	rt_status->storage_no[home_id].gyroy = 20.0;
// 	rt_status->storage_no[home_id].gyroz  = 20.0;
// 	rt_status->storage_no[home_id].aacx  = 10.0;
// 	rt_status->storage_no[home_id].aacy  = 30.0;
// 	rt_status->storage_no[home_id].aacz  = 0.01;

// 	// rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
// 	// rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;
	
// 	return 0;
// }