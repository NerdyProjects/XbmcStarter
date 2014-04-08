CFLAGS=-lmicrohttpd -lpthread
SRC=xbmcStarter.c
OBJS := $(SRC:.c=.o)
BIN=$(SRC:.c=)

all: $(BIN)

.PHONY: clean

clean:
	rm -rf $(BIN) $(OBJS)

