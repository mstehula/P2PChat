CC= gcc

WIN-C-FLAGS= -g -c -DWINDOWS
WIN-L-FLAGS= -lws2_32

LINUX-C-FLAGS= -g -c
LINUX-L-FLAGS=

all:
	echo "Cannot compile without arguments, use (make) windows or (make) linux"

windows: win-compile
	$(CC) main.o -o server $(WIN-L-FLAGS)

win-compile:
	$(CC) $(WIN-C-FLAGS) main.c $(WIN-L-FLAGS)

linux: linux-compile
	$(CC) main.o -o server $(WIN-L-FLAGS)

linux-compile:
	$(CC) $(LINUX-C-FLAGS) main.c $(LINUX-L-FLAGS)

clean:
	rm *.exe
	rm *.out
	rm *.o
