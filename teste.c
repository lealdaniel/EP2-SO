#include "list.h"

List ** laps;


int main() {
	laps = initLaps(laps, 5);

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
	addCyclistToLap(laps, 0, 1);
	cyclists[1]->lastLapTime = 5;
	addCyclistToLap(laps, 1, 1);
	cyclists[2]->lastLapTime = 6;
	addCyclistToLap(laps, 2, 1);
	cyclists[3]->lastLapTime = 6;
	cyclists[3]->broke = 1;
	addCyclistToLap(laps, 3, 1);
	cyclists[4]->lastLapTime = 7;
	cyclists[4]->broke = 1;
	addCyclistToLap(laps, 4, 1);
	int elim = eliminateLast(laps, 1, cyclists);
	printf("eliminei o %d\n", elim);
	printLaps(laps, 5);

	return 0;
}