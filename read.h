// Prototype for read data function
#ifndef READ_H
#define READ_H

#define MAX_BUFFER_SIZE 512+1
#define MAX_FIELD_SIZE 128+1

tower_t **read_data(char *filename, int *lines);

#endif