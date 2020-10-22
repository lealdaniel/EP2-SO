

ep2: main.o output.o
	gcc -g -Wall -o ep2 main.o output.o -lpthread

main.o: main.c output.o
	gcc -g -Wall -c main.c output.h -lpthread

output.o: output.c output.h
	gcc -g -Wall -c output.c output.h

