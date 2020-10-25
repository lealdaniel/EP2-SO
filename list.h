#ifndef LIST_H
#define LIST_H

#include "cyclist.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
  int id;
  struct Node * next;
} Node;

typedef struct List {
  int numLapped;
  Node * first;
  Node * last;
} List;

List ** initLaps(List ** laps, int numCyclists);
void addCyclistToLap(List ** laps, int id, int lap);
void freeLaps(List ** laps, int numCyclists);
int checkLapped(List ** laps, int numCyclists, int lap);
void printLaps(List ** laps, int numCyclists);
int eliminateLast(List ** laps, int lap, Cyclist ** cyclists);


#endif

