obj-m += hello_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm -rf *.o *.mod.c .*.cmd *.mod modules.order Module.symvers

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean