#ifndef UTILS_GRAPH_H_
#define UTILS_GRAPH_H_

typedef struct {
    unsigned s;
    unsigned t;
} edge;

typedef struct {
    unsigned n; // number of nodes
    unsigned m; // number of edges
    edge *edges; 
    unsigned *adj;
    unsigned *sadj;

} graph;

typedef struct {
    int id;
    double r;
} rank;
graph *build_graph(char *fp, int k);
void reorder(graph *g, double *r);
#endif