#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include "cyclist.h"

void outputLaps(Cyclist ** cyclists, int numCyclists);
void rankCyclists(Cyclist ** cyclists, int numCyclists);
void recordToFile(int memory, unsigned int timePastsecs, unsigned int timePastms, char * filename);

#endif