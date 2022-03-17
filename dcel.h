// Function prototypes and structs for Doubly Connected Edge List 
#ifndef DCEL_H
#define DCEL_H

typedef struct halfedge halfedge_t;
struct halfedge {
    int index_start;    // index of vertex at the start of the half-edge
    int index_end;      // index of vertex at the end of the half-edge
    halfedge_t *next;   // pointer to the next half-edge in the face
    halfedge_t *prev;   // pointer to the previous half-edge in the face
    halfedge_t *twin;   // pointer to the other half-edge (runs in the opposite direction)
    int face;           // index of its corresponding face
    int parent_edge;    // index of the edge it forms a part of
};

typedef struct {
    double x;   // x-position
    double y;   // y-position
} vertex_t;

typedef struct {
    halfedge_t *half_edge;   // pointer to either half-edge in the edge
    int size;                // size of edge array
} edge_t ;

typedef struct {
    halfedge_t *half_edge;   // pointer to any half-edge in the face
    int size;                // size of face array
} face_t;

typedef struct {
    edge_t *edges;           // array of pointers to half-edges
    vertex_t *vertices;      // array of vertices
    face_t *faces;           // arrray of faces
} poly_t;

vertex_t *store_vertices(char* polygon, int *size);
int point_inside(double x, double y, vertex_t *vertices, halfedge_t *halfedge);
void *midpoint(double *x, double*y, int index_1, edge_t *edges, vertex_t *vertices);
halfedge_t *create_halfedge(int index_start, int index_end, int face_num, int parent_edge);
void *execute_split(poly_t *poly, int *size, int *faces_size, int start_edge, int end_edge);

#endif