OBJS	= main.o util.o
SOURCE	= main.c util.c
HEADER	= util.h
OUT	= rush.exe
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

util.o: util.c
	$(CC) $(FLAGS) util.c 


clean:
	rm -f $(OBJS) $(OUT)