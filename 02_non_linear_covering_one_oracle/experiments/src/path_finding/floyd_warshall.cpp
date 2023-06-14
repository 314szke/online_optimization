#include "floyd_warshall.h"

#include "visualization/print.hpp"


FloydWarshall::FloydWarshall(const Model& model) :
    max_dimension(0)
{
    const Model::Graph_t& A = model.getAdjacencyMatrix();
    const uint32_t MAX_VALUE = model.getNbVertices() + 1;

    // Initialize distance matrix
    D.resize(model.getNbVertices());
    for (uint32_t i = 0; i < model.getNbVertices(); i++) {
        D[i].resize(model.getNbVertices());
        for (uint32_t j = 0; j < model.getNbVertices(); j++) {
            if (A[i][j] != -1) {
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
    for (uint32_t k = 0; k < model.getNbVertices(); k++) {
        for (uint32_t i = 0; i < model.getNbVertices(); i++) {
            for (uint32_t j = 0; j < model.getNbVertices(); j++) {
                if (D[i][j] > D[i][k] + D[k][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                }
            }
        }
    }

    // Initialize allowed edges
    allowed_edges.resize(model.getNbRequests());
    for (uint32_t r = 0; r < model.getNbRequests(); r++) {
        allowed_edges[r].resize(model.getNbEdges());
        for (uint32_t e = 0; e < model.getNbEdges(); e++) {
            allowed_edges[r][e] = 0.0;
        }
    }

    // Find allowed edges
    const Model::RequestVec_t& requests = model.getRequests();
    for (const Request& r : requests) {
        BoolVec_t marker(model.getNbVertices(), true);
        for (uint32_t i = 0; i < model.getNbVertices(); i++) {
            if (i == r.target) {
                continue;
            }
            if (D[i][r.target] == MAX_VALUE) {
                marker[i] = false;
            }
        }

        uint32_t counter = 0.0;
        for (uint32_t i = 0; i < model.getNbVertices(); i++) {
            for (uint32_t j = 0; j < model.getNbVertices(); j++) {
                if (A[i][j] != -1 && marker[i] && marker[j]) {
                    allowed_edges[r.id][A[i][j]] = 1.0;
                    counter++;
                }
            }
        }

        if (counter > max_dimension) {
            max_dimension = counter;
        }
    }

    //print_solution("D", 0, D);
    //print_solution("allowed_edges", max_dimension, allowed_edges);
}

const DoubleVec_t& FloydWarshall::getReachableEdges(const uint32_t request_id) const
{
    return allowed_edges[request_id];
}

uint32_t FloydWarshall::getMaxDimension() const
{
    return max_dimension;
}
