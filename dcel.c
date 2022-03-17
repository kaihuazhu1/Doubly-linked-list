#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dcel.h"

#define MAX_BUFFER_SIZE 512+1
#define MAX_FIELD_SIZE 128+1

// Stores vertices in an array from given polygon file
vertex_t *store_vertices(char* polygon, int *size){
    FILE *polyfile;
    if ((polyfile = fopen(polygon, "r")) == NULL){
        printf("Can't open file.\n");
        exit(1);
    }
    *size = 10;
    vertex_t *vertices = malloc(sizeof(vertex_t) * (*size));
    assert(vertices);

    int i=0;
    while(fscanf(polyfile, "%lf%lf", &vertices[i].x, &vertices[i].y) == 2) {
        if(i == *size) {
            *size *= 2;
            vertices = realloc(vertices, sizeof(vertex_t*) * (*size));
        }
        i++;
    }
    fclose(polyfile);
    *size = i;
    return vertices;
}

// Returns 0 if point is "inside" half-edge and returns 1 otherwise
int point_inside(double x, double y, vertex_t *vertices, halfedge_t *halfedge) {
    double start_x = vertices[halfedge->index_start].x;
    double end_x = vertices[halfedge->index_end].x;
    double start_y = vertices[halfedge->index_start].y;
    double end_y = vertices[halfedge->index_end].y;
    // case for vertical line across the y-axis
    if (start_x == end_x && start_y < end_y) {
        if (x>start_x) {
            return 0;
        }
    }
    else if (start_x == end_x && start_y > end_y) {
        if (x<start_x) {
            return 0;
        }
    }
    // case for horizontal line across x-axis
    else if (start_y == end_y && start_x > end_x) {
        if (y>start_y) {
            return 0;
        }
    }
    else if (start_y == end_y && start_x < end_x) {
        if (y<start_y) {
            return 0;
        }
    }
    // case for half-edge with non-zero gradient
    double gradient = (end_y - start_y)/(end_x - start_x);
    double y_int = end_y - gradient*end_x;
    double predicted = gradient*x + y_int;
    if (start_x<end_x && y-predicted<=0) {
        return 0;
    }
    else if (start_x>end_x && y-predicted>=0) {
        return 0;
    }
    return 1;
}

// Calculates midpoint of an edge with given start index
void *midpoint(double *x, double*y, int index, edge_t *edges, vertex_t *vertices) {
    *x = (vertices[edges[index].half_edge->index_start].x
     + vertices[edges[index].half_edge->index_end].x)/2;
    *y = (vertices[edges[index].half_edge->index_start].y
     + vertices[edges[index].half_edge->index_end].y)/2;
}

// Creates a half-edge and assigns co-ordinates, face number and index of parent edge
halfedge_t *create_halfedge(int index_start, int index_end, int face_num, int parent_edge) {
    halfedge_t *halfedge = calloc(1,sizeof(halfedge_t));
    assert(halfedge);
    halfedge->index_start = index_start;
    halfedge->index_end = index_end;
    halfedge->face = face_num;
    halfedge->parent_edge = parent_edge;
    return halfedge;
}

// Executes a given split with start and end edge on the given polygon 
void *execute_split(poly_t *poly, int *size, int *faces_size, int start_edge, int end_edge) {
    *size = (*size)+2;
    // allocate enough memory for two more vertices
    poly->vertices = realloc(poly->vertices, sizeof(vertex_t) * (*size));
    assert(poly->vertices);
    double *new_start_x, *new_start_y, *new_end_x, *new_end_y;
    new_start_x = malloc(sizeof(double));
    assert(new_start_x);
    new_start_y = malloc(sizeof(double));
    assert(new_start_y);
    new_end_x = malloc(sizeof(double));
    assert(new_end_x);
    new_end_y = malloc(sizeof(double));
    assert(new_end_y);

    // calculates and stores midpoint of start edge
    midpoint(new_start_x, new_start_y, start_edge, poly->edges, poly->vertices);

    // calculates and stores midpoint of end edge
    midpoint(new_end_x, new_end_y, end_edge, poly->edges, poly->vertices);

    // next two indices available in the vertices array   
    int midpoint_start = (*size)-2;
    int midpoint_end = (*size)-1;
    // inserts new vertices into array
    poly->vertices[midpoint_start].x = *new_start_x;
    poly->vertices[midpoint_start].y = *new_start_y;
    poly->vertices[midpoint_end].x = *new_end_x;
    poly->vertices[midpoint_end].y = *new_end_y;

    // check if edge is pointing to correct half-edge
    vertex_t *g = malloc(sizeof(vertex_t));
    g->x = (poly->vertices[midpoint_start].x + poly->vertices[midpoint_end].x)/2;
    g->y = (poly->vertices[midpoint_start].y + poly->vertices[midpoint_end].y)/2;
    // check if point g is within both half-edges
    if (point_inside(g->x, g->y, poly->vertices, poly->edges[start_edge].half_edge) == 1) {
        poly->edges[start_edge].half_edge = poly->edges[start_edge].half_edge->twin;
    }
    if (point_inside(g->x, g->y, poly->vertices, poly->edges[end_edge].half_edge) == 1) {
        poly->edges[end_edge].half_edge = poly->edges[end_edge].half_edge->twin;
    }
    free(g);

    // stores old start and end edges before fixing 
    int old_edge_end = poly->edges[start_edge].half_edge->index_end;
    int old_edge_start = poly->edges[end_edge].half_edge->index_start;
    halfedge_t *temp_1 = poly->edges[start_edge].half_edge->next;
    halfedge_t *temp_2 = poly->edges[end_edge].half_edge->prev;
    

    // fixes half-edges of start and end edge
    poly->edges[start_edge].half_edge->index_end = midpoint_start;
    poly->edges[end_edge].half_edge->index_start = midpoint_end;
    
    // create new half-edge with twin and assign mid-points as indices
    halfedge_t *halfedge = create_halfedge(midpoint_start, midpoint_end, poly->edges[start_edge].half_edge->face, poly->edges->size + 1);
    halfedge_t *halfedge_twin = create_halfedge(midpoint_end, midpoint_start, 0, poly->edges->size + 1);

    halfedge->twin = halfedge_twin;
    halfedge_twin->twin = halfedge;

    (*faces_size)++;
    poly->faces = realloc(poly->faces, sizeof(face_t) * (*faces_size));
    // update face to new joining half-edge
    poly->faces[(*faces_size)-1].half_edge = halfedge_twin;
    

    // insert new half-edge into dcel between fixed edges
    poly->edges->size++;
    poly->edges = realloc(poly->edges, sizeof(edge_t) * (poly->edges->size+1));
    assert(poly->edges);
    poly->edges[poly->edges->size].half_edge = halfedge;
    poly->edges[start_edge].half_edge->next = halfedge;
    halfedge->prev = poly->edges[start_edge].half_edge;
    poly->edges[end_edge].half_edge->prev = halfedge;
    halfedge->next = poly->edges[end_edge].half_edge;

    // create new dcel for other face
    poly->edges->size += 2;
    poly->edges = realloc(poly->edges, sizeof(edge_t) * (poly->edges->size+1));
    halfedge_t *new_half_1 = create_halfedge(halfedge_twin->index_end, old_edge_end, 0, poly->edges->size - 1); 
    halfedge_t *new_half_2 = create_halfedge(old_edge_start, halfedge_twin->index_start, 0, poly->edges->size); 
    // insert dcel (2 other half-edges) into edges array
    poly->edges[poly->edges->size - 1].half_edge = new_half_1;
    poly->edges[poly->edges->size].half_edge = new_half_2;
    
    // case for non-adjacent splits
    if (new_half_1->index_end != new_half_2->index_start) {
        new_half_1->next = temp_1;
        temp_1->prev = new_half_1;
        new_half_2->prev = temp_2;
        temp_2->next = new_half_2;
    }
    // case for adjacent splits
    else {
        new_half_1->next = new_half_2; 
        new_half_2->prev = new_half_1;
    }

    new_half_1->prev = halfedge_twin; 
    new_half_2->next = halfedge_twin; 
    halfedge_twin->next = new_half_1; 
    halfedge_twin->prev = new_half_2;

    // create new face
    poly->faces[(*faces_size)-1].half_edge = halfedge_twin;
    halfedge_twin->face = (*faces_size)-1;
    new_half_1->face = (*faces_size)-1;
    new_half_2->face = (*faces_size)-1;

    halfedge_t *extra_half_1;
    halfedge_t *extra_half_2;

    // case for splits on a half-edge pair
    if (poly->edges[start_edge].half_edge->twin != NULL) {
        int twin_face = poly->edges[start_edge].half_edge->twin->face;
        extra_half_1 = create_halfedge(new_half_1->index_end, midpoint_start, twin_face, new_half_1->parent_edge);
        
        poly->edges[start_edge].half_edge->twin->index_start = midpoint_start;
        extra_half_1->prev = poly->edges[start_edge].half_edge->twin->prev;
        poly->edges[start_edge].half_edge->twin->prev->next = extra_half_1;
        poly->edges[start_edge].half_edge->twin->prev = extra_half_1;
        extra_half_1->next = poly->edges[start_edge].half_edge->twin;
        new_half_1->twin = extra_half_1;
        extra_half_1->twin = new_half_1;
        poly->faces[twin_face].size++;
    }
    if (poly->edges[end_edge].half_edge->twin != NULL) {
        int twin_face = poly->edges[end_edge].half_edge->twin->face;
        extra_half_2 = create_halfedge(midpoint_end, new_half_2->index_start, twin_face, new_half_2->parent_edge);
        
        poly->edges[end_edge].half_edge->twin->index_end = midpoint_end;
        extra_half_2->next = poly->edges[end_edge].half_edge->twin->next;
        poly->edges[end_edge].half_edge->twin->next->prev = extra_half_2;
        poly->edges[end_edge].half_edge->twin->next = extra_half_2;
        extra_half_2->prev = poly->edges[end_edge].half_edge->twin;
        new_half_2->twin = extra_half_2;
        extra_half_2->twin = new_half_2;
        poly->faces[twin_face].size++;
    }

    int i=0;
    halfedge_t *loop_half = halfedge_twin;
    while (loop_half->index_end != halfedge_twin->index_start) {
        loop_half->face = (*faces_size)-1;
        i++;
        loop_half = loop_half->next;
    }
    loop_half->face = (*faces_size)-1;
    poly->faces[*(faces_size)-1].size = i+2;

    poly->faces[halfedge->face].half_edge = halfedge;
    poly->faces[halfedge_twin->face].half_edge = halfedge_twin;

    free(new_start_x);
    free(new_start_y);
    free(new_end_x);
    free(new_end_y);
}
