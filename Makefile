CFLAGS=-lmicrohttpd -lpthread -O2
SRC=xbmcStarter.c
OBJS := $(SRC:.c=.o)
BIN=$(SRC:.c=)

all: $(BIN)

install: all
	cp $(BIN) $(DESTDIR)/usr/bin

.PHONY: clean

clean:
	rm -rf $(BIN) $(OBJS)

