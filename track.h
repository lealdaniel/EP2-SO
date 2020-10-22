#ifndef TRACK_H
#define TRACK_H

#include "cyclist.h"

#define MAX 10
#define MAX_SIZE 1000


typedef struct Lane {
  Cyclist * spot;
} Lane;


typedef struct Track {
  Lane lane[10];

} Track;

Track track[MAX_SIZE];

#endif
