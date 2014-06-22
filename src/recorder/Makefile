LIBS = -lasound

all:  
	gcc capture.c -o capture -L/usr/lib/x86_64-linux-gnu $(LIBS)
	cp capture ../bin

clean: 
	rm -f capture ../bin/capture
