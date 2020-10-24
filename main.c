#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"
#include "output.h"
#include "unistd.h"
#include "time.h"
#include "list.h"

pthread_mutex_t mutex[MAX_SIZE][MAX];
pthread_mutex_t lap;
pthread_mutex_t timeMutex;
pthread_mutex_t barrier[MAX_SIZE];
int arrive[MAX_SIZE] = { 0 };
int distance, numCyclists;
int lapGlobal = 0;
int timePast = 0;

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane);
void * thread(void * rider);
int randomizeId(int * ids, int numCyclists);
void printTrack();

int main(int argc, char ** argv) {
  int totalCyclists = 0;
  int arrivedCyclist;
  int id;
  int ids[4*MAX_SIZE] = {0};
  int eliminated = 0;
  int debug = 0;
  pthread_t tid[MAX_SIZE];

  if (argc < 3) {
    printf("Argumentos 'd' e 'n' faltando");
    exit(1);
  }

  printf("Deseja debugar? Sim: 1. Nao: 0\n");
  scanf("%d", &debug);

  srand(time(NULL));
  distance = atoi(argv[1]);
  numCyclists = atoi(argv[2]);
  Cyclist **cyclists = malloc(numCyclists * sizeof(Cyclist));
  initLaps(numCyclists);
  pthread_mutex_init(&lap, NULL);
  pthread_mutex_init(&timeMutex, NULL);


  for (int i = 0; i < distance; i++) {
    for (int j = 0; j < 10; j++) {
      pthread_mutex_init(&mutex[i][j], NULL);

      if (totalCyclists < numCyclists && i < distance && j < 5) {
        pthread_mutex_init(&barrier[totalCyclists], NULL);

        track[i].lane[j].spot = malloc(sizeof(Cyclist));
        id = randomizeId(ids, numCyclists);
        cyclists[id] = track[i].lane[j].spot;


        track[i].lane[j].spot->id = id;
        track[i].lane[j].spot->drawnSpeed= 30;
        track[i].lane[j].spot->actualSpeed = 30;
        track[i].lane[j].spot->position = i;
        track[i].lane[j].spot->lane = j;
        track[i].lane[j].spot->broke = 0;
        track[i].lane[j].spot->eliminated = 0;
        track[i].lane[j].spot->lastLapTime = 0;
        track[i].lane[j].spot->lap = 0;

        totalCyclists++;
      }
      else 
        track[i].lane[j].spot = NULL;
    }
  }

  for(int i = 0; i < totalCyclists; i++)
    if (pthread_create(&tid[i], NULL, thread, cyclists[i])) {
      printf("Erro ao tentar criar as threads \n");
      exit(1);
    }

  while (numCyclists > 1) {

    arrivedCyclist = 1;
    for (int i = 0; i < totalCyclists && arrivedCyclist; i++) {
      if (!arrive[i])  
        arrivedCyclist = 0;
    }

    if (arrivedCyclist) {
      if (checkLapped(numCyclists, lapGlobal + 1)) {
        eliminated = 0;
        lapGlobal += 1;
      }

    
    
      if (lapGlobal != 0 && lapGlobal % 2 == 0 && !eliminated) {
        int picked = eliminateLast(lapGlobal, cyclists);
        cyclists[picked]->eliminated = 1;
        track[cyclists[picked]->position].lane[cyclists[picked]->lane].spot = NULL;
        eliminated = 1;
        numCyclists--;
      }
      
      for (int i = 0; i < totalCyclists; i++) {
        if (!cyclists[i]->eliminated && !cyclists[i]->broke) {
          arrive[cyclists[i]->id] = 0;
          pthread_mutex_unlock(&barrier[cyclists[i]->id]);
        }
      }
    }
    if (debug) {
      usleep(100000);
      printTrack();
    }  

    pthread_mutex_lock(&timeMutex);
    if (numCyclists > 2)
      timePast += 60;
    else    
      timePast += 20;
    pthread_mutex_unlock(&timeMutex);

    
  }

  rankCyclists(cyclists, totalCyclists);

  for (int i = 0; i < totalCyclists; i++) {

    pthread_mutex_unlock(&barrier[i]);
    pthread_mutex_destroy(&barrier[i]);

    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads \n");
      exit(1);
    }
  }

  for (int i = 0; i < totalCyclists; i++) {
    free(cyclists[i]);
  }
  free(cyclists);
  freeLaps(totalCyclists);
  for (int i = 0; i < distance; i++) 
    for (int j = 0; j < 10; j++) {

      pthread_mutex_destroy(&mutex[i][j]);
    }

  return 0;
}

void * thread(void * rider) {
  Cyclist * cyclist = rider;
  int t = 0;
  int currentPosition = -1, currentLane = -1, desiredPosition = -1, desiredLane = -1, previousPosition = -1;
  int cross = 0;
  Cyclist * auxCyclist;

  while (numCyclists > 1 && !cyclist->broke && !cyclist->eliminated) {
    arrive[cyclist->id] = 1;
    pthread_mutex_lock(&barrier[cyclist->id]);

    if (cyclist->lap != 0 && cyclist->lap % 2 == 0 && cross) {
      switch (cyclist->drawnSpeed) {
        case 30:
          if ((rand() % 100) < 80)
            cyclist->drawnSpeed = 60;
          else
            cyclist->drawnSpeed = 30;
          break;
        
        default:
          if ((rand() % 100) < 40)
            cyclist->drawnSpeed = 30;
          else
            cyclist->drawnSpeed = 60;
          break;  
      }
      cross = 0;
    }

    if (cyclist->lap >= 2 && cyclist->lane - 1 >= 0 && track[cyclist->position].lane[cyclist->lane - 1].spot == NULL) {
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane - 1]);
      auxCyclist = track[cyclist->position].lane[cyclist->lane - 1].spot;
      currentLane = cyclist->lane;
      currentPosition = cyclist->position;
      desiredLane = cyclist->lane-1;
      desiredPosition = cyclist->position;

      if (auxCyclist == NULL) {
        changePlace(cyclist, cyclist->position, cyclist->lane - 1);
      }
      pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
      pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
    }

    pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][cyclist->lane]);   
    int lockedPos = (cyclist->position + 1) % distance; 
    int lockedLane = cyclist->lane;
    if (track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot != NULL) {
      if (track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot->actualSpeed == 30 && cyclist->drawnSpeed == 60) {
        int overTake = 0;
        if (cyclist->lane + 1 < 10) { 
          for (int i = cyclist->lane + 1; i < 10 && !overTake; i++) {
            pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][i]);
            pthread_mutex_lock(&mutex[cyclist->position][i]);
            currentLane = cyclist->lane;
            currentPosition = cyclist->position;
            desiredPosition = (cyclist->position + 1) % distance;
            desiredLane = i;
          
            if (track[cyclist->position].lane[i].spot == NULL && track[(cyclist->position + 1) % distance].lane[i].spot == NULL) {
              pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
              auxCyclist = track[(cyclist->position + 1) % distance].lane[i].spot;
              if (auxCyclist == NULL) {
                cyclist->actualSpeed = cyclist->drawnSpeed;
                changePlace(cyclist, (cyclist->position + 1) % distance, i);
              }
              pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
              overTake = 1;
            }
            pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
            pthread_mutex_unlock(&mutex[currentPosition][i]);
          }
        }

        if (!overTake)
          cyclist->actualSpeed = 30;
        
      }
    }

    else
      cyclist->actualSpeed = cyclist->drawnSpeed;

    pthread_mutex_unlock(&mutex[lockedPos][lockedLane]);    

    if (numCyclists > 2) {
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
    }
    else {
      switch (cyclist->actualSpeed) {
        case 60:
          t += 20;
          break;

        case 90:
          t += 30;
          break;

        default:
          t += 10;
          break;
      }
    }

    if (t >= 60 && track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot == NULL) {

      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
      pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][cyclist->lane]);

      currentPosition = cyclist->position;
      currentLane = cyclist->lane;
      desiredLane = cyclist->lane;
      desiredPosition = (cyclist->position + 1) % distance;

      if (track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot == NULL) {
        previousPosition = currentPosition;
        changePlace(cyclist, (cyclist->position + 1) % distance, cyclist->lane);
        t -= 60;
      }

      pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
      pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);

    }

    if (previousPosition == distance - 1 && cyclist->position == 0) {
      previousPosition = 0;
      pthread_mutex_lock(&timeMutex);
      cyclist->lastLapTime = timePast;
      pthread_mutex_unlock(&timeMutex);
      cyclist->lap += 1;
      pthread_mutex_lock(&lap);
      addCyclistToLap(cyclist->id, cyclist->lap);
      pthread_mutex_unlock(&lap);
      cross = 1;
    }

    if (cyclist->position == 0 && cyclist->lap != 0 && cyclist->lap % 6 == 0 && numCyclists > 5) {
      if (rand() % 100 < 5) {
        pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
        printf("O ciclista%d quebrou na volta: %d\n", cyclist->id, cyclist->lap);
        cyclist->broke = 1;
        numCyclists--;
        track[cyclist->position].lane[cyclist->lane].spot = NULL;
        arrive[cyclist->id] = 1;
        pthread_mutex_unlock(&mutex[cyclist->position][cyclist->lane]);
        pthread_cancel(pthread_self());
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

void printTrack() {
  int i, j;

  for (i = 0; i < distance; i++) {
    for (j = 0; j < 10; j++) {
      if (track[i].lane[j].spot != NULL)
        printf("%3d|", track[i].lane[j].spot->id);
      else
        printf("   |");
    }
    printf("\n");
  }
  printf("\n\n\n");

}

int randomizeId(int * ids, int numCyclists) {
  int chose = 0;
  int id;

  id = rand() % numCyclists;

  if (ids[id]) {
    for (int i = id + 1; i < numCyclists && !chose; i++) {
      if (!ids[i]) {
        ids[i] = 1;
        id = i;
        chose = 1;
      }
    }

    for (int i = 0; i < id - 1 && !chose; i++) {
      if (!ids[i]) {
        ids[i] = 1;
        id = i;
        chose = 1;
      }
    }
  }

  else 
    ids[id] = 1;
   
  return id;
}


