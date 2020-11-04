#include <pthread.h>
#include "track.h"
#include "output.h"
#include "unistd.h"
#include "list.h"
#include "string.h"
#include "math.h"

pthread_mutex_t mutex[MAX_TRACK][LANES];
pthread_mutex_t lap;
pthread_mutex_t numCyclistsMutex;
pthread_mutex_t total;
pthread_mutex_t barrier[MAX_CYCLISTS];
int arrive[MAX_CYCLISTS] = { 0 };
int distance, numCyclists;
int lapGlobal = 0;
int totalLaps = 0;
unsigned long int timePastsecs = 0;
unsigned long int timePastms = 0;
int totalCyclists = 0;
List ** laps;

void changePlace(Cyclist * cyclist, int desiredPosition, int desiredLane);
void * thread(void * rider);
int randomizeId(int * ids, int numCyclists);
int getMemory();
void printTrack();

int main(int argc, char ** argv) {
  int arrivedCyclist;
  int id;
  int ids[MAX_CYCLISTS] = {0};
  int eliminated = 0;
  int debug;
  pthread_t tid[MAX_CYCLISTS];

  if (argc < 4) {
    printf("Argumentos 'd' e 'n' e 'debug' faltando\n");
    exit(1);
  }
  debug = atoi(argv[3]);


  srand(time(NULL));
  distance = atoi(argv[1]);
  numCyclists = atoi(argv[2]);
  Cyclist **cyclists = malloc(numCyclists * sizeof(Cyclist));
  laps = initLaps(laps, numCyclists);
  pthread_mutex_init(&lap, NULL);
  pthread_mutex_init(&total, NULL);
  pthread_mutex_init(&numCyclistsMutex, NULL);

  for (int i = 0; i < distance; i++) {
    for (int j = 0; j < 10; j++) {
      pthread_mutex_init(&mutex[i][j], NULL);

      if (totalCyclists < numCyclists && i < distance && j > 0 && j < 6) {
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
        track[i].lane[j].spot->finished = 0;
        track[i].lane[j].spot->lastLapTimesecs = 0;
        track[i].lane[j].spot->lastLapTimems = 0;
        track[i].lane[j].spot->lap = 0;

        totalCyclists++;
      }
      else 
        track[i].lane[j].spot = NULL;
    }
  }

  for (int i = 0; i < totalCyclists; i++)
    if (pthread_create(&tid[i], NULL, thread, cyclists[i])) {
      printf("Erro ao tentar criar as threads \n");
      exit(1);
    }

  int finished = 0;
  totalLaps = 2*(totalCyclists-1);
  while (!finished) {

    arrivedCyclist = 1;
    for (int i = 0; i < totalCyclists && arrivedCyclist; i++) {
      if (!arrive[i]) { 
        arrivedCyclist = 0;
      }
    }


    if (arrivedCyclist) {
      if (numCyclists > 2)
        timePastms += 60;
      else    
        timePastms += 20;

      if (timePastms >= 1000){
        timePastsecs += 1;
        timePastms -= 1000;
      }

      int newlap = 1;
      for (int i = 0; i < totalCyclists && newlap; i++)
        if (cyclists[i]->broke == 0 && cyclists[i]->eliminated == 0 && cyclists[i]->lap <= lapGlobal)
          newlap = 0;

      if (newlap == 1){
        lapGlobal += 1;
        eliminated = 0;
        printf("\n\nTODOS CICLISTAS COMPLETARAM A VOLTA: %d\n", lapGlobal-1);
        outputLaps(cyclists, totalCyclists);
      }
    
      if (lapGlobal != 0 && lapGlobal % 2 == 0 && !eliminated) {
        int picked = eliminateLast(laps, lapGlobal, cyclists);
        if (picked != -1) {
          cyclists[picked]->eliminated = 1;
          track[cyclists[picked]->position].lane[cyclists[picked]->lane].spot = NULL;
          eliminated = 1;
          numCyclists--;
        }
      }

      if (debug) {
        printTrack();
      }  

      if (numCyclists == 1)
        finished = 1;

      for (int i = 0; i < totalCyclists; i++) {
        if (!cyclists[i]->eliminated && !cyclists[i]->broke && !cyclists[i]->finished) {
          arrive[cyclists[i]->id] = 0;
          pthread_mutex_unlock(&barrier[cyclists[i]->id]);
        }
      }
    }  
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
  freeLaps(laps, totalCyclists);

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
  int lefted;
  int random = 0;

  while (cyclist->finished == 0 && cyclist->eliminated == 0 && cyclist->broke == 0) {
    lefted = 0;

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
          if ((rand() % 100) < 20)
            cyclist->drawnSpeed = 30;
          else
            cyclist->drawnSpeed = 60;
          break;  
      }
      cross = 0;
    }

    if (cyclist->lane - 1 >= 0) {
      currentLane = cyclist->lane;
      currentPosition = cyclist->position;
      desiredLane = cyclist->lane-1;
      desiredPosition = cyclist->position;
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane - 1]);
      if (cyclist->lap >= 2  && track[cyclist->position].lane[cyclist->lane - 1].spot == NULL) {
        lefted = 1;
        pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
        changePlace(cyclist, cyclist->position, cyclist->lane - 1);
        pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
      }
      pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
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
            currentLane = cyclist->lane;
            currentPosition = cyclist->position;
            desiredPosition = (cyclist->position + 1) % distance;
            desiredLane = i;
          
            if (track[cyclist->position].lane[i].spot == NULL && track[(cyclist->position + 1) % distance].lane[i].spot == NULL) {
              pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
              cyclist->actualSpeed = cyclist->drawnSpeed;
              changePlace(cyclist, (cyclist->position + 1) % distance, i);
              pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
              overTake = 1;
            }
            pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
          }
        }

        if (!overTake)
          cyclist->actualSpeed = 30;
      }
    }

    else
      cyclist->actualSpeed = cyclist->drawnSpeed;

    pthread_mutex_unlock(&mutex[lockedPos][lockedLane]);    

    pthread_mutex_lock(&numCyclistsMutex);
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
    pthread_mutex_unlock(&numCyclistsMutex);


    currentPosition = cyclist->position;
    currentLane = cyclist->lane;
    desiredPosition = (cyclist->position + 1) % distance;
    desiredLane = cyclist->lane;

    if (t >= 60) {
      pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
      if (track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot == NULL) {
          pthread_mutex_lock(&mutex[(cyclist->position + 1) % distance][cyclist->lane]);
          if (track[(cyclist->position + 1) % distance].lane[cyclist->lane].spot == NULL) {
            previousPosition = currentPosition;
            changePlace(cyclist, (cyclist->position + 1) % distance, cyclist->lane);
            t -= 60;
          }

          pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
      }

      else if (lefted == 0 && lapGlobal >= 2 && cyclist->lane + 1 < 10) {
        pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane + 1]);
        desiredPosition = cyclist->position;
        desiredLane = cyclist->lane + 1;
        if (track[desiredPosition].lane[desiredLane].spot == NULL)
          changePlace(cyclist, desiredPosition, desiredLane);
        pthread_mutex_unlock(&mutex[desiredPosition][desiredLane]);
      }
      pthread_mutex_unlock(&mutex[currentPosition][currentLane]);
    }

    if (previousPosition == distance - 1 && cyclist->position == 0) {
      previousPosition = 0;
      cyclist->lap += 1;
      cyclist->lastLapTimesecs = timePastsecs;
      cyclist->lastLapTimems = timePastms;
      random = 0;

      if (cyclist->lastLapTimems >= 1000){
        cyclist->lastLapTimesecs += 1;
        cyclist->lastLapTimems -= 1000;
      }
      pthread_mutex_lock(&total);
      if (cyclist->lap <= totalLaps) {
        pthread_mutex_lock(&lap);
        addCyclistToLap(laps, cyclist->id, cyclist->lap);
        pthread_mutex_unlock(&lap);
      }
      else{
        arrive[cyclist->id] = 1;
        cyclist->finished = 1;
        pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
        track[cyclist->position].lane[cyclist->lane].spot = NULL;        
        pthread_mutex_unlock(&mutex[cyclist->position][cyclist->lane]);
      }
      pthread_mutex_unlock(&total);

      cross = 1;
    }


    pthread_mutex_lock(&numCyclistsMutex);
    if (random == 0 && !cyclist->finished && cyclist->position == 0 && cyclist->lap != 0 && cyclist->lap % 6 == 0 && numCyclists > 5) {
      if (rand()%100  < 5) {
        pthread_mutex_lock(&mutex[cyclist->position][cyclist->lane]);
        printf("O ciclista%d quebrou após completar a volta: %d\n", cyclist->id, cyclist->lap-1);
        cyclist->broke = 1;
        numCyclists--;
        pthread_mutex_lock(&total);
        totalLaps -= 2;
        pthread_mutex_unlock(&total);
        track[cyclist->position].lane[cyclist->lane].spot = NULL;
        pthread_mutex_unlock(&mutex[cyclist->position][cyclist->lane]);
        random = 1;
      }
    }
    pthread_mutex_unlock(&numCyclistsMutex);

    pthread_mutex_lock(&numCyclistsMutex);
    if (numCyclists == 1){
      arrive[cyclist->id] = 1;
      cyclist->finished = 1;
    }
    pthread_mutex_unlock(&numCyclistsMutex);
  }

  if (cyclist->broke) {
    arrive[cyclist->id] = 1;
    pthread_cancel(pthread_self());
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

    for (int i = 0; i < id && !chose; i++) {
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


int getMemory() {
  FILE * output;
  char buffer[1000];
  char * memory;
  unsigned int total = 0;

  output = fopen("/proc/self/statm", "r");

  fread(buffer, 1000, 1, output);

  memory = strtok(buffer, " ");
  total = atoi(memory);

  // for (int i = 0; i < 4; i++)
  //   memory = strtok(NULL, " ");

  // memory = strtok(NULL, " ");
  // total += atoi(memory);

  fclose(output);

  return total;
}