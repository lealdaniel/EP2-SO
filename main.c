#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"

pthread_mutex_t mutex[MAX_SIZE][MAX];
pthread_barrier_t barrier[2];
int numCyclist = 0;
int distance, numCyclists;
int crossed = 0;
int bar = -1;

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane);
void * thread(void * rider);
void printTrack();

int main(int argc, char ** argv) {
  int num = 0;
  int s, lapGlobal = 0;
  pthread_t tid[MAX_SIZE];

  if (argc < 3) {
    printf("Argumentos 'd' e 'n' faltando");
    exit(1);
  }

  distance = atoi(argv[1]);
  numCyclists = atoi(argv[2]);
  pthread_barrier_init(&barrier[0], NULL, numCyclists);
  Cyclist **cyclists = malloc(numCyclists * sizeof(Cyclist));
  // lembrar de dar free
  // inicializa a posições dos ciclistas e a matriz
  // falta randomizar

  for (int i = 0; i < distance; i++) {
    for (int j = 0; j < 10; j++) {
      pthread_mutex_init(&mutex[i][j], NULL);

      if (num < numCyclists && i < distance && j < 5) {

        track[i].lane[j].spot = malloc(sizeof(Cyclist));
        cyclists[num] = track[i].lane[j].spot;

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
      else 
        track[i].lane[j].spot = NULL;
    }
  }

  // o coordenador deve ver quem é o ultimo checando quem está no zero e eliminá-lo
  // if lapglobal % 2 == 0 and crossed == numcyclist -> elimina
  for(int i = 0; i < numCyclists; i++)
    if (pthread_create(&tid[i], NULL, thread, cyclists[i])) {
      printf("Erro ao tentar criar as threads \n");
      exit(1);
    }

  while (numCyclists > 1) {
    bar = -1;
    if (lapGlobal % 2 == 0 && crossed >= numCyclists) {

      int count = 0;
      for (int i = 0 ; i < 10; i++)
        if (track[0].lane[i].spot != NULL)
          count += 1;

      int picked = rand() % count;

      for(int j = picked; j < 10; j++)
        if (track[0].lane[j].spot != NULL && track[0].lane[j].spot->lap == lapGlobal){
          picked = j;
          break;
        }

      int id = track[0].lane[picked].spot->id;

      track[0].lane[picked].spot->eliminated = 1;
      track[0].lane[picked].spot = NULL;

      // destroi a thread do eleminado

      pthread_cancel(tid[id]);

      numCyclists--;
      if (bar == -1){
        pthread_barrier_init(&barrier[1], NULL, numCyclists);
        pthread_barrier_destroy(&barrier[0]);
      }
      else {
        pthread_barrier_init(&barrier[0], NULL, numCyclists);
        pthread_barrier_destroy(&barrier[1]);
      }

      bar = -bar;
      crossed = 0;
    }
    printTrack();
  }


  for(int i = 0; i < numCyclists; i++)
    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads \n");
      exit(1);
    }

  for (int i = 0; i < distance; i++)
    for (int j = 0; j < 10; j++) 
      pthread_mutex_destroy(&mutex[i][j]);

  if (bar == -1)
    pthread_barrier_destroy(&barrier[0]);
  else 
    pthread_barrier_destroy(&barrier[1]);


  return 0;
}

void * thread(void * rider) {
  Cyclist * cyclist = rider;
  int t = 0;
  int aux;

  if (cyclist->position == 0) // se ele começou no 0, subtrai uma lap pra evitar complicações
    cyclist->lap = -1;

  while (!cyclist->broke && !cyclist->eliminated && numCyclists > 1) {
    Cyclist * nextCyclist = track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot;

    if (cyclist->lap >= 2) {
      switch (cyclist->previousSpeed) {

        case 30:
          if (rand() % 1 < 0.8)
            cyclist->currentSpeed = 60;
          else
            cyclist->currentSpeed = 30;
          break;
        
        default:
          if (rand() % 1< 0.4)
            cyclist->currentSpeed = 30;
          else
            cyclist->currentSpeed = 60;
          break;
        }
    }

    // se a lane da esquerda existe e nao tem ninguem la
    if (cyclist->lane - 1 >= 0 && track[cyclist->position].lane[cyclist->lane - 1].spot == NULL) {
      aux = cyclist->lane;
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane - 1]);
      changePlace(cyclist, cyclist->position, cyclist->lane - 1);
      pthread_mutex_unlock(&mutex[cyclist->position][aux]);
    }
    
    if (nextCyclist != NULL) {
      if (nextCyclist != NULL && nextCyclist->actualSpeed == 30) {
        // se o cara esta a 60, a lane do lado existe e esta vazia
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
    if (numCyclists > 2){
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

    if (t >= 60) {
      aux = cyclist->position;
      pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][cyclist->lane]);
      changePlace(cyclist, (cyclist->position + 1) % distance, cyclist->lane);
      pthread_mutex_unlock(&mutex[aux][cyclist->lane]);
      cyclist->position = (cyclist->position + 1) % distance;
      t -= 60;
    }

    if (cyclist->position == 0){
      cyclist->lap += 1;
      crossed += 1;
    }

    // código para ver se o cara foi eliminado
    if (cyclist->lap % 6 && numCyclists > 5) {
      if (rand() % 1 < 0.05) {
        cyclist->broke = 1;
        numCyclists--;
        // avisar que quebrou
        printf("quebrei\n");
        // destruir a thread
        // ele atinge a barreira, sai do loop e se destroi no final
        // dar free no ciclista
        track[cyclist->position].lane[cyclist->lane].spot = NULL;
        free(cyclist);
      }
    }
    if (bar == -1)
      pthread_barrier_wait(&barrier[0]);
    else
      pthread_barrier_wait(&barrier[1]);
  }

  pthread_cancel(pthread_self());

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

  for (i = 0; i < 10; i++) {
    for (j = 0; j < distance; j++) {
      if (track[i].lane[j].spot != NULL)
        printf("%3d|", track[i].lane[j].spot->id);
      else
        printf("   |");
    }
    printf("\n");
  }
}