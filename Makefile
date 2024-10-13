# Variables
CC = gcc
CFLAGS = -Wall -g

# Targets
all: cgi-bin server

cgi-bin: random.c
	$(CC) $(CFLAGS) -o cgi-bin random.c

server: server.c
	$(CC) $(CFLAGS) -o server server.c

# Clean target to remove compiled files
clean:
	rm -f cgi-bin server && rm -rf cgi-bin.dSYM
