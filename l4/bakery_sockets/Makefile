.PHONY: all s c clean

INC := bakery.h request.h response.h

all: server client

s:
	make server
	./server

c:
	make client
	./client

server: server.c $(INC)
	gcc -g server.c -o server

client: client.c $(INC)
	gcc -g client.c -o client

clean:
	rm server
	rm client
