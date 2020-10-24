

ep2: main.o output.o list.o
	gcc -g -Wall -o ep2 main.o list.o output.o -lpthread

main.o: main.c output.o list.o
	gcc -g -Wall -c main.c output.h list.h -lpthread

output.o: output.c output.h
	gcc -g -Wall -c output.c output.h

list.o: list.c list.h
	gcc -g -Wall -c list.c list.h

