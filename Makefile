OBJS	= main.o util.o
SOURCE	= main.c, util.c,
HEADER	= util.h
OUT	= a.out
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c,
	$(CC) $(FLAGS) main.c, -std=c11

util.o: util.c,
	$(CC) $(FLAGS) util.c, -std=c11


clean:
	rm -f $(OBJS) $(OUT)