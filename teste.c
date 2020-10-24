#include "list.h"

int main() {
	initLaps(5);

  Cyclist **cyclists = malloc(5 * sizeof(Cyclist));
	for (int j = 0; j < 5; j++) {
		cyclists[j] = malloc(sizeof(Cyclist)); 
		cyclists[j]->id = j;
		cyclists[j]->drawnSpeed= 30;
		cyclists[j]->actualSpeed = 30;
		cyclists[j]->position = 0;
		cyclists[j]->lane = j;
		cyclists[j]->broke = 0;
		cyclists[j]->eliminated = 0;
		cyclists[j]->lastLapTime = 0;
		cyclists[j]->lap = 0;
	}

	cyclists[0]->lastLapTime = 5;
	addCyclistToLap(0, 0);
	cyclists[1]->lastLapTime = 5;
	addCyclistToLap(1, 0);
	cyclists[2]->lastLapTime = 5;
	addCyclistToLap(2, 0);
	cyclists[3]->lastLapTime = 5;
	addCyclistToLap(3, 0);
	cyclists[4]->lastLapTime = 5;
	addCyclistToLap(4, 0);
	int elim = eliminateLast(0, cyclists);
	printf("eliminei o %d\n", elim);
	printLaps(5);

	return 0;
}