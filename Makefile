# Variables
CC = gcc
CFLAGS = -Wall -g

# Targets
all: cgi-bin server.o

cgi-bin: random.c
	$(CC) $(CFLAGS) -o cgi-bin random.c

server.o: server.c
	$(CC) $(CFLAGS) -c -o server.o server.c

# Clean target to remove compiled files
clean:
	rm -f cgi-bin server.o
