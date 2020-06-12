CC = gcc
CFLAGS =
CLIBS = 
CMDS  = p t

all : $(CMDS)

p : multi_process.c  
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS) -pthread

t : multi_thread.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS) -pthread

clean :
	rm $(CMDS) core 

