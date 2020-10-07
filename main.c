#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"
#include <semaphore.h>

#define MAX 10
#define MAX_SIZE 1000

sem_t spots[MAX_SIZE][MAX]; 
pthread_mutex_t mutex;

void * thread(void * cyclist) {
  Cyclist * cyclist = cyclist
}

int main(int argc, char ** argv) {
  Track track[MAX_SIZE];
  int d, n;
  int num = 0;
  
  if (argc < 3) {
    printf("Argumentos 'd' e 'n' faltando");
    exit(1);
  }

  d = atoi(argv[1]);
  n = atoi(argv[2]);

  for (int i = 0; i < d && num < n; i++) {
    for (int j = 0; j < 5; j++) {
      track[i].cyclist[j].id = num;
      track[i].cyclist[j].currentSpeed = 30;
      track[i].cyclist[j].actualSpeed = 30;
      track[i].cyclist[j].position = 0;
      track[i].cyclist[j].broke = 0;
      track[i].cyclist[j].eliminated = 0;
      num++;
    }
  }

  return 1;
}