all: server

server: server.o
	gcc server.o -o server -pthread

server.o: server.c
	gcc -c server.c -g

clean:
	rm -f *.o server
