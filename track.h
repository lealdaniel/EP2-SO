#ifndef TRACK_H
#define TRACK_H

#include "cyclist.h"

#define LANES 10
#define MAX_TRACK 2500
#define MAX_CYCLISTS 100


typedef struct Lane {
  Cyclist * spot;
} Lane;


typedef struct Track {
  Lane lane[LANES];

} Track;

Track track[MAX_TRACK];

#endif
