#include "uart_app.h"

int set_com_config(int fd,int baud_rate,int data_bits,char parity,int stop_bits)
{
    struct termios new_cfg,old_cfg;
    int speed;
    /*保存原有串口配置*/
    if(tcgetattr(fd,&old_cfg)!=0)
    {
        perror("tcgetattr");
        return -1;
    }

    new_cfg = old_cfg;

    /*配置为原始模式*/
    cfmakeraw(&new_cfg);
    new_cfg.c_cflag &= ~CSIZE;

    /*设置波特率*/
    switch(baud_rate)
    {
        case 4800:
            speed = B4800;
            break;
        case 9600:
            speed = B9600;
            break;
        case 38400:
            speed = B38400;
            break;
        default:
        case 115200:
            speed = B115200;
            break;
    }

    /*设置输入输出波特率*/
    cfsetispeed(&new_cfg,speed);
    cfsetospeed(&new_cfg,speed);

    /*设置数据位*/
    switch(data_bits)
    {
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        default:
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
    }

    /*设置奇偶校验位*/
    switch(parity)
    {
        default:
        case 'n':
        case 'N':
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            new_cfg.c_cflag |= (PARODD | PARENB);
            new_cfg.c_iflag |= INPCK;
            break;
    }

    /*设置停止位*/
    switch(stop_bits)
    {
        default:
        case 1:
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2:
            new_cfg.c_cflag |= CSTOPB;
            break;
    }

    /*设置等待时间和最小接收字符*/
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 1;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&new_cfg))!=0)
    {
        perror("tcsetattr");
        return -1 ;
    }

    return 0;
}

int recvDats(int fd,unsigned char *pBuf,int datalen)
{
    int curlen = 0;
    int reallen = 0;
    while(curlen < datalen)
    {
        reallen = read(fd,pBuf+curlen,datalen-curlen);
        if(reallen >= 0)
            curlen+=reallen;
        else
            return -1;
    }
    return 0;
}