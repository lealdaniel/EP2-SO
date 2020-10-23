#include "output.h"

void outputLaps(Cyclist ** cyclists, int numCyclists) {
  for (int i = 0; i < numCyclists; i++) {
    if (!cyclists[i]->eliminated && !cyclists[i]->broke)
      printf("Ciclista%d está na volta %d, na posição %d\n", cyclists[i]->id, cyclists[i]->lap, cyclists[i]->position);
  }
}

void rankCyclists(Cyclist ** cyclists, int numCyclists) {
  Cyclist ** broke = malloc(numCyclists * sizeof(Cyclist));
  Cyclist ** notBroke = malloc(numCyclists * sizeof(Cyclist));
  Cyclist * aux;
  int numBroke = 0;
  int numNotBroke = 0;

  for (int i = 0; i < numCyclists; i++) {
    if (cyclists[i]->broke)
      broke[numBroke++] = cyclists[i];
    else {
      notBroke[numNotBroke++] = cyclists[i];
    }
  }

  for (int i = 1; i < numBroke; i++) {
    for (int j = 0; j < numBroke - i; j++) {
      if (broke[j]->lap < broke[j + 1]->lap) {
        aux = broke[j];
        broke[j] = broke[j + 1];
        broke[j + 1] = aux;
      }
    }
  }

  for (int i = 1; i < numNotBroke; i++) {
    for (int j = 0; j < numNotBroke - i; j++) {
      if (notBroke[j]->lap < notBroke[j + 1]->lap) {
        aux = notBroke[j];
        notBroke[j] = notBroke[j + 1];
        notBroke[j + 1] = aux;
      }
    }
  }
  
  for (int i = 0; i < numNotBroke; i++) {
    printf("#%d ciclista%d, cruzou a linha de chegada no instante %d pela ultima vez\n", i + 1, notBroke[i]->id, notBroke[i]->lastLapTime);
  }

  printf("\n");

  for (int i = 0; i < numBroke; i++) {
    printf("O ciclista%d quebrou na volta %d\n", broke[i]->id, broke[i]->lap);
  }
}
