SRCS	= cam.c demacbin.c
HDRS	= cam2jpgtab.h
TARGET	= camdump camtoppm camtojpeg demacbin
OBJS	?= OBJS
CFLAGS	?= -g -O2
LIBS	= 
bindir	= ${exec_prefix}/bin
CC	= clang-21
LDCC ?= $(CC)

all:	$(TARGET)

camdump:	cam.c
	$(LDCC) $(CFLAGS) -DCAMDUMP -o camdump cam.c

camtoppm:	cam.c
	$(LDCC) $(CFLAGS) -DCAMTOPPM -o camtoppm cam.c

camtojpeg:	cam.c cam2jpgtab.h
	$(LDCC) $(CFLAGS) -DCAMTOJPEG -o camtojpeg cam.c

demacbin:	demacbin.c
	$(LDCC) $(CFLAGS) -o demacbin demacbin.c

install:
	install -c -s -o bin -g bin -m 555 camdump $(bindir)/camdump
	install -c -s -o bin -g bin -m 555 camtoppm $(bindir)/camtoppm
	install -c -s -o bin -g bin -m 555 camtojpeg $(bindir)/camtojpeg
	install -c -s -o bin -g bin -m 555 demacbin $(bindir)/demacbin

clean:
	/bin/rm $(TARGET) $(OBJS)

.PHONY: all clean install camdump camtoppm camtojpeg demacbin
