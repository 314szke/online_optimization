#include "CBFS.h"


CBFS::CBFS(const adjacency_t& A) :
    _graph(A),
    visited(A.size()),
    prev(A.size())
{}

UIntVec_t CBFS::getPath(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    UIntVec_t path;

    if (pathExists(s, t, marker)) {
        uint32_t i = t;
        uint32_t j = prev[t];
        path.push_back(_graph[j][i]);

        while (j != s) {
            i = j;
            j = prev[i];
            path.push_back(_graph[j][i]);
        }
    }

    return path;
}

bool CBFS::pathExists(const uint32_t s, const uint32_t t, const BoolVec_t& marker)
{
    // Initialize path search
    UIntVec_t Q;
    for (uint32_t idx = 0; idx < _graph.size(); idx++) {
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
        for (uint32_t j = 0; j < _graph.size(); j++) {
            if (_graph[i][j] != -1 && marker[_graph[i][j]]) {
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
