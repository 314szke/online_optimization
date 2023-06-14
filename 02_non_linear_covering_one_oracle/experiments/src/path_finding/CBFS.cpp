#include "CBFS.h"

#include "visualization/print.hpp"


CBFS::CBFS(const Model& model) :
    _model(model),
    visited(model.getNbVertices()),
    prev(model.getNbVertices())
{}

UIntVec_t CBFS::getPath(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    const Model::Graph_t& A = _model.getAdjacencyMatrix();

    UIntVec_t path;

    if (pathExists(s, t, marker)) {
        uint32_t i = t;
        uint32_t j = prev[t];
        path.push_back(A[j][i]);

        while (j != s) {
            i = j;
            j = prev[i];
            path.push_back(A[j][i]);
        }
    }

    return path;
}

bool CBFS::pathExists(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    const Model::Graph_t& A = _model.getAdjacencyMatrix();

    // Initialize path search
    UIntVec_t Q;
    for (uint32_t idx = 0; idx < _model.getNbVertices(); idx++) {
        visited[idx] = false;
        prev[idx] = -1;
    }

    Q.push_back(s);
    visited[s] = true;
    uint32_t idx = 0;
    uint32_t i;

    // Find path with marked edges
    while (idx < Q.size()) {
        i = Q[idx];
        for (uint32_t j = 0; j < _model.getNbVertices(); j++) {
            if (A[i][j] != -1 && marker[A[i][j]]) {
                if (j == t) {
                    prev[j] = i;
                    return true;
                }
                if (! visited[j]) {
                    prev[j] = i;
                    Q.push_back(j);
                    visited[j] = true;
                }
            }
        }
        idx++;
    }

    return false;
}
