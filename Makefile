CC := /usr/bin/gcc
SERVERCFLAGS :=
SERVEROBJECTS := build/game.o
CLIENTCFLAGS :=
CLIENTOBJECTS :=

ifdef PROTO
ifeq ($(PROTO), TCP)
	SERVERCFLAGS += -DTCP
	SERVEROBJECTS += build/tcp.o
endif
ifeq ($(PROTO), UDP)
	SERVERCFLAGS += -DUDP
	SERVEROBJECTS += build/udp.o
endif
else
	SERVERCFLAGS += -DUDP
	SERVEROBJECTS += build/udp.o
endif

all: client server | build

server: $(SERVEROBJECTS) server.c | build
	@$(CC) $(SERVERCFLAGS) $^ -o server

test: $(SERVEROBJECTS) $(CLIENTOBJECTS) test.c | build
	@$(CC) $(SERVERCFLAGS) $(CLIENTOBJECTS) -g $^ -o test

build/%.o: src/%.c src/%.h | build
	@$(CC) $(SERVERCFLAGS) $(CLIENTCFLAGS) -c -g $< -o $@

client: $(CLIENTOBJECTS) client.c | build
	@$(CC) $(CLIENTCFLAGS) $^ -o client

build:
	@mkdir build

.PHONY: clean

clean:
	@rm -rf build server test client