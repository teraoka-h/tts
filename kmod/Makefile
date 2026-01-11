SRC = tts_timer.c
obj-m := ${SRC:.c=.o}

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean