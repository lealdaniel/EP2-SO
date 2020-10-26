#ifndef CYCLIST_H
#define CYCLIST_H

typedef struct Cyclist {
  int id;
  int drawnSpeed; // atual velocidade sorteada
  int actualSpeed; // velocidade efetiva
  int position;
  int lane;
  int broke;
  int eliminated;
  int lap;
  unsigned long int lastLapTime;
  
} Cyclist;

#endif