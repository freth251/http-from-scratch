# Variables
CC = gcc
CFLAGS = -Wall -g

# Targets
all: cgi-bin server

cgi-bin: random.c
	$(CC) $(CFLAGS) -o cgi-bin random.c

server.o: server.c
	$(CC) $(CFLAGS) -c -o server.o server.c

server: server.o
	$(CC) $(CFLAGS) -o server server.o

# Clean target to remove compiled files
clean:
	rm -f cgi-bin server.o server && rm -rf cgi-bin.dSYM
