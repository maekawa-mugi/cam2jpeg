SRCS	= cam.c demacbin.c
HDRS	= cam2jpgtab.h
TARGET	= camdump camtoppm camtojpeg demacbin
OBJS	=
DESTDIR	= /usr/local/bin
# for BSD/OS 2.0
CC	= cc
LDCC	= shlicc
# for other systems
#CC	= cc
#LDCC	= $(CC)

all:	$(TARGET)

camdump:	cam.c
	$(LDCC) $(CFLAGS) -DCAMDUMP -o camdump cam.c

camtoppm:	cam.c
	$(LDCC) $(CFLAGS) -DCAMTOPPM -o camtoppm cam.c

camtojpeg:	cam.c cam2jpgtab.h
	$(LDCC) $(CFLAGS) -DCAMTOJPEG -o camtojpeg cam.c

demacbin:	demacbin.c
	$(LDCC) $(CFLAGS) -o demacbin demacbin.c

.if defined(DESTDIR)
install:
	install -c -s -o bin -g bin -m 555 camdump $(DESTDIR)/camdump
	install -c -s -o bin -g bin -m 555 camtoppm $(DESTDIR)/camtoppm
	install -c -s -o bin -g bin -m 555 camtojpeg $(DESTDIR)/camtojpeg
	install -c -s -o bin -g bin -m 555 demacbin $(DESTDIR)/demacbin
.endif

clean:
	/bin/rm $(TARGET) $(OBJS)

shar:	cam2jpeg.shar
cam2jpeg.shar:	README.eng README Makefile $(SRCS) $(HDRS) magic.diff
	shar -T README.eng README Makefile $(SRCS) $(HDRS) magic.diff > cam2jpeg.shar
