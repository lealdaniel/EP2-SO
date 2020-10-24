#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
List ** initLaps(List ** laps, int numCyclists) {
  laps = (List**) malloc((2 * numCyclists + 1) * sizeof(List*));
  
  for (int i = 1; i < 2 * numCyclists + 1; i++) {
    laps[i] = (List*) malloc(sizeof(List));
    laps[i]->first = NULL;
    laps[i]->last = NULL;
    laps[i]->numLapped = 0;
  }
  return laps;
}

void addCyclistToLap(List ** laps, int id, int lap) {

  Node * aux = malloc(sizeof(Node));
  // printf("%d\n", lap);  
  aux->id = id;
  aux->next = NULL;
  printf("%d\n", lap);
  laps[lap]->numLapped += 1;
  if (laps[lap]->first == NULL) {    
    laps[lap]->first = aux;
    laps[lap]->last = aux;
  }

  else {
    aux->next = laps[lap]->last;
    laps[lap]->last = aux;
  }
}

int checkLapped(List ** laps, int numCyclists, int lap) {
  printf("%p\n", laps);
  if (laps[lap]->numLapped == numCyclists) {
    return 1;
  }
    
  return 0;
}

void freeLaps(List ** laps, int numCyclists) {
  Node * aux;
  for (int i = 1; i < (2 * numCyclists + 1); i++) {
    aux = laps[i]->last;
    while (aux != NULL) {
      laps[i]->last = aux->next;
      free(aux);
      aux = laps[i]->last;
    }
    free(laps[i]);
  }

  free(laps);
}

void printLaps(List ** laps, int numCyclists) {
  Node * aux;
  
  for (int i = 1; i < (2 * numCyclists + 1); i++) {
    aux = laps[i]->last;
    printf("volta: %d\n", i+1);
    while (aux != NULL) {
      printf("id: %d\n", aux->id);
      aux = aux->next;
    }
  }
}

int eliminateLast(List ** laps, int lap, Cyclist ** cyclists) {
  int count = 0;
  Node * aux = laps[lap]->last;
  int lastTime = cyclists[aux->id]->lastLapTime;

  srand(time(NULL));

  while(aux != NULL && cyclists[aux->id]->lastLapTime == lastTime) {
    count++;
    aux = aux->next;
  }

  if (count > 0) {
    int picked = rand() % count;
    aux = laps[lap]->last;
    for (int i = 1; i < picked; i++)
      aux = aux->next;

    return aux->id;
  }

  return laps[lap]->last->id;

}