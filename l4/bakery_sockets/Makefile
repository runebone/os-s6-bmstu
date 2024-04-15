.PHONY: all s c clean sts

INC := bakery.h request.h response.h

all: server client single_threaded_server

s:
	make server
	./server

sts:
	make single_threaded_server
	./sts

c:
	make client
	./client

server: server.c $(INC)
	gcc -g server.c -o server

single_threaded_server: single_threaded_server.c $(INC)
	gcc -g single_threaded_server.c -o sts

client: client.c $(INC)
	gcc -g client.c -o client

clean:
	rm server
	rm client
