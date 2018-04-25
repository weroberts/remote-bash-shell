output: server.o client.o
	gcc server.o -o server
	gcc client.o -o client
server: chef.c
	gcc -c server.c -o server.o
client: client.c
	gcc -c client.c -o client.o
