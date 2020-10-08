#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"

#define RAND_MAX 1

pthread_mutex_t mutex[MAX_SIZE][MAX];
int numCyclist = 0;
int d, n;

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane);
void * thread(void * rider);

int main(int argc, char ** argv) {
  int num = 0;

  if (argc < 3) {
    printf("Argumentos 'd' e 'n' faltando");
    exit(1);
  }

  d = atoi(argv[1]);
  n = atoi(argv[2]);
  initLanes(d);

  for (int i = 0; i < d && num < n; i++) {
    for (int j = 0; j < 10; j++) {
      pthread_mutex_init(&mutex[i][j], NULL);

      if (j < 5) {
        track[i].lane[j].spot = malloc(sizeof(Cyclist));
        track[i].lane[j].spot->id = num;
        track[i].lane[j].spot->currentSpeed= 30;
        track[i].lane[j].spot->actualSpeed = 30;
        track[i].lane[j].spot->position = i;
        track[i].lane[j].spot->lane = j;
        track[i].lane[j].spot->broke = 0;
        track[i].lane[j].spot->eliminated = 0;
        pthread_mutex_lock(&mutex[i][j]);
        num++;
      }

    }
  }


  return 0;
}

void * thread(void * rider) {
  Cyclist * cyclist = rider;
  int lap = 0;
  int t = 0;
  int aux;

  while (!cyclist->broke || !cyclist->eliminated) {
    Cyclist * nextCyclist = track[(cyclist->position + 1) % d].lane[cyclist->lane].spot;

    if (lap >= 2) {
      switch (cyclist->previousSpeed) {

      case 30:
        if (rand() < 0.8)
          cyclist->currentSpeed = 60;
        else
          cyclist->currentSpeed = 30;
        break;
      
      default:
        if (rand() < 0.4)
          cyclist->currentSpeed = 30;
        else
          cyclist->currentSpeed = 60;
        break;
      }
    }

    if (cyclist->lane - 1 >= 0 && track[cyclist->position].lane[cyclist->lane - 1].spot == NULL) {
      aux = cyclist->lane;
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane - 1]);
      changePlace(cyclist, cyclist->position, cyclist->lane - 1);
      pthread_mutex_unlock(&mutex[cyclist->position][aux]);
    }
    
    if (nextCyclist != NULL) {
      if (nextCyclist != NULL && nextCyclist->actualSpeed == 30) {
        if (cyclist->currentSpeed == 60 && cyclist->lane + 1 < 10 && track[cyclist->position].lane[cyclist->lane + 1].spot == NULL) { 
          aux = cyclist->lane;
          pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane + 1]);
          cyclist->actualSpeed = cyclist->currentSpeed;
          changePlace(cyclist, cyclist->position, cyclist->lane + 1);
          pthread_mutex_unlock(&mutex[cyclist->position][aux]);
        }

        else
          cyclist->actualSpeed = 30;

        
      }
    }

    else
      cyclist->actualSpeed = cyclist->currentSpeed;





    // fim da volta
    cyclist->previousSpeed = cyclist->currentSpeed;

    switch (cyclist->actualSpeed) {
      case 60:
        t += 60;
        break;

      case 90:
        t += 90;
        break;

      default:
        t += 30;
        break;
    }

    if (t >= 60) {
      aux = cyclist->position;
      pthread_mutex_lock(&mutex[cyclist->position + 1][cyclist->lane]);
      changePlace(cyclist, cyclist->position + 1, cyclist->lane);
      pthread_mutex_unlock(&mutex[aux][cyclist->lane]);
      cyclist->position += 1;
      t -= 60;
    }


    // código para ver se o cara foi eliminado

    if (cyclist->position % 6 && n > 5) {
      if (rand() < 0.05) {
        cyclist->broke = 1;
        n--;
        // avisar que quebrou
        // destruir a thread
        // dar free no ciclista
        track[cyclist->position].lane[cyclist->lane].spot = NULL;
      }
    }
  }

  return NULL;
}

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane) {
  track[desiredPosition].lane[desiredLane].spot = cyclist;
  track[cyclist->position].lane[cyclist->lane].spot = NULL;
  cyclist->position = desiredPosition;
  cyclist->lane = desiredLane;
}
