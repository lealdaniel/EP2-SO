#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"
#include "unistd.h"

pthread_mutex_t mutex[MAX_SIZE][MAX];
pthread_mutex_t barrier[MAX_SIZE];
int arrive[MAX_SIZE] = { 0 };
int distance, numCyclists;
int crossed = 0;
int lapGlobal = 0;

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane);
void * thread(void * rider);
void printTrack();

int main(int argc, char ** argv) {
  int totalCyclists = 0;
  int arrivedCyclist;
  pthread_t tid[MAX_SIZE];

  if (argc < 3) {
    printf("Argumentos 'd' e 'n' faltando");
    exit(1);
  }

  distance = atoi(argv[1]);
  numCyclists = atoi(argv[2]);
  Cyclist **cyclists = malloc(numCyclists * sizeof(Cyclist));
  // lembrar de dar free
  // inicializa a posições dos ciclistas e a matriz
  // falta randomizar

  for (int i = 0; i < distance; i++) {
    for (int j = 0; j < 10; j++) {
      pthread_mutex_init(&mutex[i][j], NULL);

      if (totalCyclists < numCyclists && i < distance && j < 5) {
        pthread_mutex_init(&barrier[totalCyclists], NULL);

        track[i].lane[j].spot = malloc(sizeof(Cyclist));
        cyclists[totalCyclists] = track[i].lane[j].spot;

        track[i].lane[j].spot->id = totalCyclists;
        track[i].lane[j].spot->drawnSpeed= 30;
        track[i].lane[j].spot->actualSpeed = 30;
        track[i].lane[j].spot->position = i;
        track[i].lane[j].spot->lane = j;
        track[i].lane[j].spot->broke = 0;
        track[i].lane[j].spot->eliminated = 0;
        track[i].lane[j].spot->lap = 0;

        totalCyclists++;
      }
      else 
        track[i].lane[j].spot = NULL;
    }
  }
  // o coordenador deve ver quem é o ultimo checando quem está no zero e eliminá-lo
  // if lapglobal % 2 == 0 and crossed == numcyclist -> elimina
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
      int leastLap = 10e6;
      for (int i = 0; i < totalCyclists; i++) {
        if (!cyclists[i]->eliminated && !cyclists[i]->broke && cyclists[i]->lap < leastLap)
          leastLap = cyclists[i]->lap;
      }
      lapGlobal = leastLap;

      printf("crossed %d lapglobal %d\n", crossed, lapGlobal);
    
      if (lapGlobal != 0 && lapGlobal % 2 == 0) { // && crossed >= numCyclists) {

        int count = 0;
        for (int i = 0 ; i < 10; i++)
          if (track[0].lane[i].spot != NULL && track[0].lane[i].spot->lap == lapGlobal)
            count += 1;

        if (count > 0){
          int picked = rand() % count;
          for (int j = picked; j < 10; j++)
            if (track[0].lane[j].spot != NULL && track[0].lane[j].spot->lap == lapGlobal) {
              picked = j;
              printf("id %d\n", track[0].lane[j].spot->id);
              break;
            }

          // int id = track[0].lane[picked].spot->id; // seg fault aqui as vezes
          printf("picked %d\n", picked);
          sleep(10);
          printf("eliminei %d\n", track[0].lane[picked].spot->id);
          // sleep(4);
          track[0].lane[picked].spot->eliminated = 1;
          track[0].lane[picked].spot = NULL;

          numCyclists--;
          
          // crossed = 0;
        }

      }
      
      printf("liberando da barreira volta %d\n", lapGlobal);
      for (int i = 0; i < totalCyclists; i++) {
        if (!cyclists[i]->eliminated && !cyclists[i]->broke){
          arrive[i] = 0;
          pthread_mutex_unlock(&barrier[i]);
        }
      }
    }
    usleep(100000);
    printTrack();  
    // printf("lap: %d\n", lapGlobal);

  }

  for (int i = 0; i < totalCyclists; i++) {

    pthread_mutex_unlock(&barrier[i]);
    pthread_mutex_destroy(&barrier[i]);

    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads \n");
      exit(1);
    }
    free(cyclists[i]);
  }
  free(cyclists);

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
  Cyclist * nextCyclist, * auxCyclist;

  while (numCyclists > 1 && !cyclist->broke && !cyclist->eliminated && numCyclists > 1) {
    nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;

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

    // se a lane da esquerda existe e nao tem ninguem la
    if (cyclist->lap >= 2 && cyclist->lane - 1 >= 0 && track[cyclist->position].lane[cyclist->lane - 1].spot == NULL) {
      // printf("travei ao esquerdar %d\n", cyclist->id);
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
      // printf("migue %d\n", cyclist->id);
      nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;
    }
    
    if (nextCyclist != NULL) {
      if (nextCyclist->actualSpeed == 30 && cyclist->drawnSpeed == 60) {
        int overTake = 0;
        if (cyclist->lane + 1 < 10) { 
          for (int i = cyclist->lane + 1; i < 10 && !overTake; i++) {
            if (track[cyclist->position].lane[i].spot == NULL && track[(cyclist->position + 1) % distance].lane[i].spot == NULL) {
              // printf("travei ao ultrapassar %d\n", cyclist->id);
              pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][i]);
              pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
              auxCyclist = track[(cyclist->position + 1) % distance].lane[i].spot;
              currentLane = cyclist->lane;
              currentPosition = cyclist->position;
              desiredPosition = (cyclist->position + 1) % distance;
              desiredLane = i;
              if (auxCyclist == NULL) {
                cyclist->actualSpeed = cyclist->drawnSpeed;
                changePlace(cyclist, (cyclist->position + 1) % distance, i);
              }
              pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
              pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
              // printf("migue %d\n", cyclist->id);
              nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;
              overTake = 1;
            }
          }
        }

        if (!overTake)
          cyclist->actualSpeed = 30;
        
      }
    }

    else
      cyclist->actualSpeed = cyclist->drawnSpeed;

    
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

    if (t >= 60 && nextCyclist == NULL) {

      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
      pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][cyclist->lane]);

      nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;
      currentPosition = cyclist->position;
      currentLane = cyclist->lane;
      desiredLane = cyclist->lane;
      desiredPosition = (cyclist->position + 1) % distance;

      if (nextCyclist == NULL) {
        previousPosition = currentPosition;
        changePlace(cyclist, (cyclist->position + 1) % distance, cyclist->lane);
        t -= 60;
      }

      pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
      pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);

      nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;
      // printf("migue %d\n", cyclist->id);
    }

    if (previousPosition == distance - 1 && cyclist->position == 0) {
      previousPosition = 0;
      // if (cyclist->lap == lapGlobal)
      //   crossed += 1;
      cyclist->lap += 1;
      cross = 1;
    }

    // código para ver se o cara foi quebrou
    if (cyclist->lap != 0 && cyclist->lap % 6 == 0 && numCyclists > 5) {
      if (rand() % 100 < 5) {
        printf("quebrei %d\n", cyclist->id);
        // sleep(3);
        cyclist->broke = 1;
        numCyclists--;
        // avisar que quebrou
        track[cyclist->position].lane[cyclist->lane].spot = NULL;
        arrive[cyclist->id] = 1;
        // free(cyclist);
        pthread_cancel(pthread_self());
      }
    }
    printf("ciclista %d volta %d\n", cyclist->id, cyclist->lap);
  }
  // pthread_cancel(pthread_self());

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