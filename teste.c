// #include <stdio.h>
// #include <stdlib.h>
#include "list.h"

int main() {
	initLaps(5);

	addCyclistToLap(1, 1);
	addCyclistToLap(2, 1);
	addCyclistToLap(3, 1);
	addCyclistToLap(4, 1);
	addCyclistToLap(5, 1);
	printLaps(5);

	return 0;
}