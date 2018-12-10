SRCPATH=../src
OBJPATH=../obj
HDRPATH=../inc
LIBPATH=../lib

CC = gcc
CFLAGS = -O -Wall

INCLUDES = -I$(SRCPATH) -I$(HDRPATH)
VPATH = $(SRCPATH)

SRCS=$(wildcard $(SRCPATH)/*.c)

HEADERS=$(wildcard $(HDRPATH)/*.h)
NOTDIRSRCS=$(notdir $(SRCS))
OBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(NOTDIRSRCS))
STATICLIB=$(LIBPATH)/librx.a

ifneq (${CROSS_COMPILE},)

ifeq (${CROSS_COMPILE},x86_64-w64-mingw32-)
CC = $(CROSS_COMPILE)gcc
WIN_LIBS := -lws2_32 -lwinmm -lgdi32
STROPHE_CFLAGS := $(shell pkg-config --cflags "libstrophe")
STROPHE_LIBS := $(shell pkg-config --libs "libstrophe")

CURL_LIBS := $(shell pkg-config --libs --static "libcurl")
CURL_CFLAGS := $(shell pkg-config --cflags "libcurl") -DCURL_STATICLIB

OPENSSL_LIBS := $(shell pkg-config --libs "openssl")
OPENSSL_CFLAGS := $(shell pkg-config --cflags "openssl")

EXPAT_LIBS := $(shell pkg-config --libs "expat")
EXPAT_CFLAGS := $(shell pkg-config --cflags "expat")

CFLAGS = -Wall -Werror -Wno-unused -DBUILD_WIN_DLL $(OPENSSL_CFLAGS) $(EXPAT_CFLAGS) $(STROPHE_CFLAGS) $(CURL_CFLAGS) $(JSONC_CFLAGS)
LDFLAGS = -static $(STROPHE_LIBS) $(OPENSSL_LIBS) $(EXPAT_LIBS) $(CURL_LIBS) $(WIN_LIBS) $(JSONC_LIBS) -lpthread
SHAREDLIB=$(LIBPATH)/rx.dll

endif

ifeq (${CROSS_COMPILE},$(filter ${CROSS_COMPILE},arm-linux-gnueabihf- arm-buildroot-linux-uclibcgnueabihf-))

CC = $(CROSS_COMPILE)gcc
STROPHE_CFLAGS := $(shell pkg-config --cflags "libstrophe")
STROPHE_LIBS := $(shell pkg-config --libs "libstrophe")

CURL_LIBS := $(shell pkg-config --libs --static "libcurl")
CURL_CFLAGS := $(shell pkg-config --cflags "libcurl") -DCURL_STATICLIB

OPENSSL_LIBS := $(shell pkg-config --libs "openssl")
OPENSSL_CFLAGS := $(shell pkg-config --cflags "openssl")

EXPAT_LIBS := $(shell pkg-config --libs "expat")
EXPAT_CFLAGS := $(shell pkg-config --cflags "expat")

CFLAGS = -fPIC -Wall -Werror -Wno-unused $(OPENSSL_CFLAGS) $(EXPAT_CFLAGS) $(STROPHE_CFLAGS) $(CURL_CFLAGS) $(JSONC_CFLAGS)
LDFLAGS = -static $(STROPHE_LIBS) $(OPENSSL_LIBS) $(EXPAT_LIBS) $(CURL_LIBS) $(JSONC_LIBS) -lpthread
SHAREDLIB=
#SHAREDLIB=$(LIBPATH)/librx.so

endif

endif

all: library

library: $(ROBJPATH) $(XOBJPATH) $(LIBPATH) $(STATICLIB) ${SHAREDLIB}

$(LIBPATH):
	mkdir $@

${STATICLIB}: ${OBJS}
	${AR} crs $@ $^
	@echo Compile $@ successful....

${SHAREDLIB}: ${OBJS}
	${CC} -o $@ -shared $^
	@echo Compile $@ successful....

$(OBJS): $(OBJPATH)/%.o: %.c
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) -c -o $@  $< $(INCLUDES)

clean:
	rm -vrf $(OBJPATH) $(LIBRX) $(LIBPATH)

