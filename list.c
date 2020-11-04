#include "list.h"
#include "unistd.h"


List ** initLaps(List ** laps, int numCyclists) {
  laps = (List**) malloc((2 * numCyclists) * sizeof(List*));
  
  for (int i = 1; i < 2 * numCyclists; i++) {
    laps[i] = (List*) malloc(sizeof(List));
    laps[i]->first = NULL;
    laps[i]->last = NULL;
  }
  return laps;
}

void addCyclistToLap(List ** laps, int id, int lap) {
  Node * aux = malloc(sizeof(Node));
  aux->id = id;
  aux->next = NULL;

  if (laps[lap]->first == NULL) {    
    laps[lap]->first = aux;
    laps[lap]->last = aux;
  }

  else {
    aux->next = laps[lap]->last;
    laps[lap]->last = aux;
  }
}

void freeLaps(List ** laps, int numCyclists) {
  Node * aux;
  for (int i = 1; i < (2 * numCyclists); i++) {
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
  for (int i = 1; i < (2 * numCyclists); i++) {
    aux = laps[i]->last;
    printf("volta: %d\n", i);
    while (aux != NULL) {
      printf("id: %d\n", aux->id);
      aux = aux->next;
    }
  }
}

int eliminateLast(List ** laps, int lap, Cyclist ** cyclists) {
  int count = 0;
  int valid = 0;
  int lastLap;
  int lastTimesecs, lastTimems;
  Node * aux = laps[lap]->last;
  Node * aux2;

  while (aux != NULL && (cyclists[aux->id]->broke || cyclists[aux->id]->eliminated) && cyclists[aux->id]->lap) {
    aux = aux->next;
  } 

  if (aux == NULL)
    return -1;

  lastTimesecs = cyclists[aux->id]->lastLapTimesecs;
  lastTimems = cyclists[aux->id]->lastLapTimems;
  lastLap = cyclists[aux->id]->lap;

  srand(time(NULL));

  aux2 = aux;
  while(aux2 != NULL && cyclists[aux2->id]->lap == lastLap && 
    cyclists[aux2->id]->lastLapTimesecs == lastTimesecs && lastTimems == cyclists[aux2->id]->lastLapTimems) {
    count++;
    aux2 = aux2->next;
  }

  if (count > 1) {
    while (!valid) {
      int picked = rand() % count;
      aux2 = aux;
      for (int i = 0; i < picked; i++)
        aux2 = aux2->next;
      if (!cyclists[aux->id]->broke && !cyclists[aux->id]->eliminated)
        valid = 1;
    }

    return aux2->id;
  }

  return aux->id;

}
