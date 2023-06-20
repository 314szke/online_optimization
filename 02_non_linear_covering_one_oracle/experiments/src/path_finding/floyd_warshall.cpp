#include "floyd_warshall.h"

#include "visualization/print.hpp"


FloydWarshall::FloydWarshall(const Model& model) :
    max_dimension(0)
{
    const uint32_t MAX_VALUE = model.graph.nb_vertices + 1;

    // Initialize distance matrix
    D.resize(model.graph.nb_vertices);
    for (uint32_t i = 0; i < model.graph.nb_vertices; i++) {
        D[i].resize(model.graph.nb_vertices);
        for (uint32_t j = 0; j < model.graph.nb_vertices; j++) {
            if (model.graph.A[i][j]) {
                D[i][j] = 1;
            } else {
                D[i][j] = MAX_VALUE;
            }
            if (i == j) {
                D[i][j] = 0;
            }
        }
    }

    // Calculate distances
    for (uint32_t k = 0; k < model.graph.nb_vertices; k++) {
        for (uint32_t i = 0; i < model.graph.nb_vertices; i++) {
            for (uint32_t j = 0; j < model.graph.nb_vertices; j++) {
                if (D[i][j] > D[i][k] + D[k][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                }
            }
        }
    }

    // Initialize allowed edges
    allowed_edges.resize(model.nb_requests);
    for (uint32_t r = 0; r < model.nb_requests; r++) {
        allowed_edges[r].resize(model.graph.nb_edges);
        for (uint32_t e = 0; e < model.graph.nb_edges; e++) {
            allowed_edges[r][e] = 0.0;
        }
    }

    // Find allowed edges
    for (const Request& r : model.requests) {
        BoolVec_t marker(model.graph.nb_vertices, true);
        for (uint32_t i = 0; i < model.graph.nb_vertices; i++) {
            if (i == r.target) {
                continue;
            }
            if (D[i][r.target] == MAX_VALUE) {
                marker[i] = false;
            }
        }

        uint32_t counter = 0.0;
        for (uint32_t i = 0; i < model.graph.nb_vertices; i++) {
            for (uint32_t j = 0; j < model.graph.nb_vertices; j++) {
                if (model.graph.A[i][j] && marker[i] && marker[j]) {
                    allowed_edges[r.id][model.graph.A[i][j]->id] = 1.0;
                    counter++;
                }
            }
        }

        if (counter > max_dimension) {
            max_dimension = counter;
        }
    }

}

const DoubleVec_t& FloydWarshall::getReachableEdges(const uint32_t request_id) const
{
    return allowed_edges[request_id];
}

uint32_t FloydWarshall::getMaxDimension() const
{
    return max_dimension;
}
