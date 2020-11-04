

ep2: main.o output.o list.o
	gcc -g -Wall main.o list.o output.o -o ep2 -lpthread -lm

main.o: main.c track.h cyclist.h
	gcc -g -Wall -c main.c -lpthread

output.o: output.c output.h
	gcc -g -Wall -c output.c

list.o: list.h list.c
	gcc -g -Wall -c list.c
clean:
	rm *.o
