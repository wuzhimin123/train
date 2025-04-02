CROSS_COMPILE=arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
CFLAGS= -c -g
LDFLAGS= -lpthread

OBJS=main.o data_global.o pthread_all.o sem.o uart_app.o

smart_storage :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp smart_storage /home/wzm/linux/nfs/rootfs

.PYTHON:clean
clean:
	rm *.o smart_storage -rf
