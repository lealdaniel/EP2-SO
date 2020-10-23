#ifndef LIST_H
#define LIST_H

#include "stdlib.h"
#include "stdio.h"

typedef struct Node {
  int id;
  struct Node * next;
} Node;

typedef struct List {
  int numLapped;
  Node * first;
  Node * last;
} List;

List ** laps;

void initLaps(int numCyclists);
void addCyclistToLap(int id, int lap);
void freeLaps(int numCyclists);
int checkLapped(int numCyclists, int lap);
void printLaps(int numCyclists, int lap);


#endif

