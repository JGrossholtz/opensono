APP_NAME = opensono
LIBS = -lasound
BUILDROOT_PATH=../../raspberry-pi/buildroot
BUILDROOT_CROSS_CC=$(BUILDROOT_PATH)/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc
BUILDROOT_LIBS=-L$(BUILDROOT_PATH)/output/target/usr/lib32

OBJECTS=	main.o \
			network_server.o\
			util.o\

all: $(OBJECTS)
	gcc -o $(APP_NAME) $^ -L/usr/lib/x86_64-linux-gnu $(LIBS)
	cp $(APP_NAME) ../bin

rbpi:
	$(BUILDROOT_CROSS_CC) player.c -o rbpi_player $(BUILDROOT_LIBS) $(LIBS) 

%.o: %.c
	gcc -o $@ -c $< $(CFLAGS)

clean: 
	@rm -f $(APP_NAME) *.o  ../bin/$(APP_NAME)


