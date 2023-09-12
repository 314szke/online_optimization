#include "graph.h"

#include <algorithm>


Graph::Graph() :
    nb_vertices(0),
    nb_edges(0)
{}

void Graph::initialize(uint32_t n, uint32_t m)
{
    nb_vertices = n;
    nb_edges = m;

    A.resize(nb_vertices);
    for (uint32_t idx = 0; idx < nb_vertices; idx++) {
        A[idx].resize(nb_vertices);
    }

    distances.resize(nb_vertices, UIntVec_t(nb_vertices, (nb_vertices + 1)));
    connections.resize(nb_vertices, IntVec_t(nb_vertices, -1));
}

// Execute Floyd-Warshall Algorithm
void Graph::findAllPaths()
{
    for (uint32_t e = 0; e < nb_edges; e++) {
        distances[ID[e].i][ID[e].j] = 1;
        connections[ID[e].i][ID[e].j] = ID[e].i;
    }

    for (uint32_t v = 0; v < nb_vertices; v++) {
        distances[v][v] = 0;
        connections[v][v] = v;
    }

    for (uint32_t k = 0; k < nb_vertices; k++) {
        for (uint32_t i = 0; i < nb_vertices; i++) {
            for (uint32_t j = 0; j < nb_vertices; j++) {
                if (distances[i][j] > distances[i][k] + distances[k][j]) {
                    distances[i][j] = distances[i][k] + distances[k][j];
                    connections[i][j] = connections[k][j];
                }
            }
        }
    }
}

Graph::Path_t Graph::getPath(uint32_t s, uint32_t t)
{
    Path_t edges;

    if (connections[s][t] == -1)
        return edges;

    uint32_t v = t;
    while (s != t) {
        t = connections[s][t];
        edges.push_back(A[t][v].id);
        v = t;
    }

    std::reverse(edges.begin(), edges.end());
    return edges;
}
