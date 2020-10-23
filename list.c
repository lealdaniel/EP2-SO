#include "list.h"

void initLaps(int numCyclists) {
  laps = (List**) malloc(2 * numCyclists * sizeof(List*));
  
  for (int i = 0; i < 2 * numCyclists; i++) {
    laps[i] = (List*) malloc(sizeof(List));
    laps[i]->first = NULL;
    laps[i]->last = NULL;
    laps[i]->numLapped = 0;
  }
}

void addCyclistToLap(int id, int lap) {
  Node * aux = malloc(sizeof(Node));
  aux->id = id;
  aux->next = NULL;
  laps[lap]->numLapped += 1;

  if (laps[lap]->first == NULL) {
    laps[lap]->first = aux;
    laps[lap]->last = aux;
  }

  else {
    laps[lap]->last->next = aux;
    laps[lap]->last = aux;
  }
}

int checkLapped(int numCyclists, int lap) {
  if (laps[lap]->numLapped == numCyclists)
    return 1;
    
  return 0;
}

void freeLaps(int numCyclists) {
  Node * aux;
  for (int i = 0; i < 2*numCyclists; i++) {
    aux = laps[i]->first;
    while (aux != NULL) {
      laps[i]->first = aux->next;
      free(aux);
      aux = laps[i]->first;
    }
    free(laps[i]);
  }

  free(laps);
}

void printLaps(int numCyclists) {
  Node * aux;
  
  for (int i = 0; i < 2*numCyclists; i++) {
    aux = laps[i]->first;
    printf("volta: %d\n", i+1);
    while (aux != NULL) {
      printf("id: %d\n", aux->id);
      aux = aux->next;
    }
  }
}