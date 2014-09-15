APP_NAME_X86 = opensono_x86
APP_NAME_ARM = opensono_arm

LIBS = -lasound -pthread
BUILDROOT_PATH=../../raspberry-pi/buildroot
BUILDROOT_CROSS_CC=$(BUILDROOT_PATH)/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc
BUILDROOT_LIBS=-L$(BUILDROOT_PATH)/output/target/usr/lib32

OBJECTS=	main.o \
			network_server.o\
			network_client.o\
			capture.o\
			player.o

all:
	$(MAKE) x86 CC=gcc
	@rm -f *.o
	$(MAKE) arm CC=$(BUILDROOT_CROSS_CC)

x86: $(OBJECTS)
	$(CC) -o $(APP_NAME_X86) $^ -L/usr/lib/x86_64-linux-gnu $(LIBS)
	@echo "X86 build OK"


arm: $(OBJECTS)
	$(CC) -o $(APP_NAME_ARM) $^  $(BUILDROOT_LIBS) $(LIBS) 
	@echo "ARM build OK"

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	@rm -f $(APP_NAME_ARM) $(APP_NAME_X86)  *.o  


