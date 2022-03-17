// Structure and function prototypes for watchtower
#ifndef TOWER_H
#define TOWER_H

typedef struct {
    char *id;
    char *name;
    char *postcode;
    int population;
    double x, y;
} tower_t;


char* take_id(tower_t *tower);

#endif