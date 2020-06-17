CC = gcc
CFLAGS = 
CLIBS = 
CMDS = Bclient Bserver

all : $(CMDS)

Bclient : bakara_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS) -lpthread -W

Bserver : bakara_server.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS) -lpthread -W

clean :
	rm $(CMDS) core
