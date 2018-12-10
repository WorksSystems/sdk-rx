TARGET=rxtest

#CROSS_COMPILE=arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc

RESTLIBS=-lcurl
XMPPLIBS=-lstrophe -lssl -lcrypto -lexpat -lresolv
BASELIBS=-lm -lpthread -ldl

INCS=-I../inc
LIBS=-L../lib -lrx $(RESTLIBS) $(XMPPLIBS) $(BASELIBS)

CFLAGS+=-Wno-deprecated-declarations

ifeq (${CROSS_COMPILE},arm-linux-gnueabihf-)
LIBS+=-Wl,-rpath,../arm-libs -L../arm-libs
endif


OBJS=main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
.c.o:
	$(CC) $(CFLAGS) $(INCS) -o $@ -c $<

clean:
	rm -vf $(OBJS) $(TARGET)

