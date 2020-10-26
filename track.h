#ifndef TRACK_H
#define TRACK_H

#include "cyclist.h"

#define LANES 10
#define MAX_TRACK 25000
#define MAX_CYCLISTS 1000


typedef struct Lane {
  Cyclist * spot;
} Lane;


typedef struct Track {
  Lane lane[LANES];

} Track;

Track track[MAX_TRACK];

#endif
