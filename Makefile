all: server client

server:
	gcc server.c -o server.o

client:
	gcc client.c -o client.o

clean:
	rm -f *.o