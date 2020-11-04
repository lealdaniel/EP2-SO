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
  int finished;
  int lap;
  unsigned long int lastLapTimemin;
  unsigned long int lastLapTimesecs;
  unsigned long int lastLapTimems;
  
} Cyclist;

#endif