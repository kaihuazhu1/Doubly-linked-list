#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tower.h"
#include "read.h"

// Reads data from file into an array of (pointers to) structs
tower_t **read_data(char* filename, int *lines) {
    FILE *data_file;
    if((data_file = fopen(filename, "r")) == NULL){ 
        printf("Can't open file.\n");
        exit(1);
    }

    char header[MAX_BUFFER_SIZE];
    fgets(header, MAX_BUFFER_SIZE, data_file);

    tower_t **towers;
    int i = 0, size = 100;
    towers = malloc(sizeof(tower_t) * size);
    assert(towers);

    char line[MAX_BUFFER_SIZE];
    // stores each row of file in "line" and stores information in (pointer to) struct
    while(fgets(line, MAX_BUFFER_SIZE, data_file) != NULL) {
        // reallocate memory for towers every 25 lines
        if (size >= 125) {
            towers = realloc(towers, sizeof(tower_t) * (i+size));
            assert(towers);
            size = 100;
        }
        tower_t *tower = malloc(sizeof(tower_t));
        assert(tower);

        // extract first token
        tower->id = (char*) malloc(sizeof(char) * MAX_FIELD_SIZE); // memory allocated if member is str type
        strcpy(tower->id, strtok(line, ","));

        // extract the remaining tokens
        tower->postcode = (char*) malloc(sizeof(char) * MAX_FIELD_SIZE);
        strcpy(tower->postcode, strtok(NULL, ","));

        tower->population = atoi(strtok(NULL, ","));

        tower->name = (char*) malloc(sizeof(char) * MAX_FIELD_SIZE);
        strcpy(tower->name, strtok(NULL, ","));

        tower->x = atof(strtok(NULL, ","));
        tower->y = atof(strtok(NULL, ","));

        // store pointer to struct in array of pointers "towers"
        towers[i] = tower;

        i++;
        size++;  
    }
    *lines = i;
    fclose(data_file);
    return towers;
}