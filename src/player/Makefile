LIBS = -lasound
BUILDROOT_PATH=../../raspberry-pi/buildroot
BUILDROOT_CROSS_CC=$(BUILDROOT_PATH)/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc
BUILDROOT_LIBS=-L$(BUILDROOT_PATH)/output/target/usr/lib32


all:  
	gcc player.c -o player -L/usr/lib/x86_64-linux-gnu $(LIBS)
	cp player ../bin

rbpi:
	$(BUILDROOT_CROSS_CC) player.c -o rbpi_player $(BUILDROOT_LIBS) $(LIBS) 

clean: 
	@rm -f player ../bin/player


