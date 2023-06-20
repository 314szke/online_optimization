#include "CBFS.h"

#include "visualization/print.hpp"


CBFS::CBFS(const Model& model) :
    _model(model),
    visited(model.graph.nb_vertices),
    prev(model.graph.nb_vertices)
{}

UIntVec_t CBFS::getPath(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    UIntVec_t path;

    if (pathExists(s, t, marker)) {
        uint32_t i = t;
        uint32_t j = prev[t];
        path.push_back(_model.graph.A[j][i]->id);

        while (j != s) {
            i = j;
            j = prev[i];
            path.push_back(_model.graph.A[j][i]->id);
        }
    }

    return path;
}

bool CBFS::pathExists(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    // Initialize path search
    UIntVec_t Q;
    for (uint32_t idx = 0; idx < _model.graph.nb_vertices; idx++) {
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
        for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
            if (_model.graph.A[i][j] && marker[_model.graph.A[i][j]->id]) {
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
