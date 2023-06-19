#include "graph.h"


Graph::Graph() :
    nb_vertices(0),
    nb_edges(0)
{}

void initialize(uint32_t n, uint32_t m)
{
    nb_vertices = n;
    nb_edges = m;

    A.resize(n);
    for (uint32_t idx = 0; idx < nb_vertices; idx++) {
        A[idx].resize(n);
    }
}
