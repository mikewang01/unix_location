#set up compiler and related compile parameter
CC=gcc

CFLAGS += -I/usr/include/json-c -I/usr/include/i386-linux-gnu/ -I/usr/include/ -I/usr/local/ssl/lib
LDFLAGS+= -pthread -D_SVID_SOURCE -std=gnu99 -L/usr/lib  -lssl -lcrypto -ljson-c

#debug needs to be excluded so as to run makefile in degug dir in last step
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
#无需下一行的注释代码，因为我们已经知道debug里的makefile是最后执行的，所以最后直接去debug目录下执行指定的makefile文件就行，具体下面有注释
#DEBUG=$(shell ls -l | grep ^d | awk '{if($$9 == "debug") print $$9}')
#record root directory
ROOT_DIR=$(shell pwd)
#excutable file name  which can be name by ur own will
BIN=serialtoeth.bin
#目标文件所在的目录
OBJS_DIR=debug/obj
#bin文件所在的目录
BIN_DIR=debug/bin
#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_SOURCE=${wildcard *.c}
#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CFLAGS LDFLAGS
#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) DEBUG 
#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
    #直接去debug目录下执行makefile文件
	make -C debug
ECHO:
	@echo $(SUBDIRS)
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.c
	$(CC) -o $(ROOT_DIR)/$(OBJS_DIR)/$@ -c $^  $(LDFLAGS) $(CFLAGS)
CLEAN:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*
