#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "dcel.h"
#include "tower.h"
#include "read.h"

// Returns watchtowers and population of each face when splits 
// are given as inputs by the user in stdin
int main(int argc, char **argv) {
    tower_t **towers;
    int *lines = malloc(sizeof(int));
    assert(lines);
    towers = read_data(argv[1], lines);

    char *file = argv[2];
    int *size = malloc(sizeof(int));
    assert(size);
    vertex_t *vertices = store_vertices(file, size);
    // store number of vertices/edges in n
    int n = *size;
    edge_t *edges = malloc(sizeof(edge_t) * n);
    assert(edges);
    edges->size = n-1;
    // create array of edges
    for(int i=0; i<n; i++) {
        halfedge_t *halfedge = create_halfedge(i, i+1, 0, i);
        halfedge->twin = NULL;
        edges[i].half_edge = halfedge;
    }
    // point head to tail
    edges[n-1].half_edge->index_end = 0;
    edges[0].half_edge->prev = edges[n-1].half_edge;
    // link half-edges
    for(int i=0; i<n-1; i++) {
        edges[i].half_edge->next = edges[i+1].half_edge;
        if(i!=0) {
            edges[i].half_edge->prev = edges[i-1].half_edge;
        }
        else {
            edges[i].half_edge->prev = NULL;
        }
    }
    // point tail to head
    edges[n-1].half_edge->next = edges[0].half_edge;

    int *faces_size = malloc(sizeof(int));
    assert(faces_size);
    *faces_size = 1;
    face_t *faces = malloc(sizeof(face_t) * (*faces_size));
    assert(faces);
    faces->half_edge = edges[0].half_edge;
    faces->size = *faces_size;
    faces[0].size = (*size)-1;

    poly_t *poly = malloc(sizeof(poly_t));
    assert(poly);
    poly->edges = edges;
    poly->faces = faces;
    poly->vertices = vertices;

    int start_edge=0, end_edge=0;

    // reads stdin if file is redirected successfully
    if ((fseek(stdin, 0, SEEK_END), ftell(stdin)) > 0) {
        rewind(stdin);
        while(scanf("%d%d", &start_edge, &end_edge)!=EOF) {
            execute_split(poly, size, faces_size, start_edge, end_edge);
        }
    }

    int *populations = malloc(sizeof(int) * (*faces_size));
    assert(populations);
    FILE *fp = fopen(argv[3], "w");

    // for each face and each line check if face includes watchtower
    for(int i=0;i<*faces_size;i++) {
        fprintf(fp, "%d\n", i);
        populations[i] = 0;
        // for each line check if each half-edge includes watchtower
        for(int j=0;j<=(*lines)-1;j++) {
            int count = 0;
            halfedge_t *half = poly->faces[i].half_edge;
            // traverse through each half-edge of the face
            for(int k=0;k<poly->faces[i].size;k++) {
                // printf("face %d start index %d end index %d\n", i, half->index_start, half->index_end);
                if(point_inside(towers[j]->x, towers[j]->y, poly->vertices, half) == 0) {
                    count++;
                }
                half = half->next;
            }
            if(count == poly->faces[i].size) {
                fprintf(fp, "Watchtower ID: %s, Postcode: %s, Population Served: %d, Watchtower Point of Contact Name: %s, x: %f, y: %f\n", 
                towers[j]->id, towers[j]->postcode, towers[j]->population, towers[j]->name, towers[j]->x, towers[j]->y);
                populations[i] += towers[j]->population;
            }
        }
    }
    for(int i=0;i<*faces_size;i++) {
        fprintf(fp, "Face %d population served: %d\n", i, populations[i]);
    }
    fclose(fp);

    for(int i=0; i<*lines; i++) {
        if (towers[i] != NULL) {
            free(towers[i]->id);
            free(towers[i]->postcode);
            free(towers[i]->name);
            free(towers[i]);
        }
    }
    free(towers);
    free(lines);

    for(int i=0;i<=poly->edges->size;i++) {
        if (poly->edges[i].half_edge->twin != NULL) {
            free(poly->edges[i].half_edge->twin);
        }
        if(poly->edges[i].half_edge != NULL) {
            free(poly->edges[i].half_edge);
        }
    }
    free(poly->vertices);
    free(poly->edges);
    free(poly->faces);
    free(poly);

    free(size);
    free(faces_size);
    free(populations);
    return 0;
}
