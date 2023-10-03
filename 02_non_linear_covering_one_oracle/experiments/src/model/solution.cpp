#include "solution.h"

#include <limits>
#include <set>


Solution::Solution(const Model& model, const DoubleVec_t& edges, uint32_t s, uint32_t t) :
    _model(model)
{
    // Find all paths used by the CP solution
    UIntMat_t all_paths = findAllPaths(s, t, edges);

    // Convert vertices to edges
    uint32_t i, j, e;
    for (uint32_t idx = 0; idx < all_paths.size(); idx++) {
        paths.push_back(UIntVec_t());
        vertices.push_back(UIntVec_t());

        double smallest_amount = std::numeric_limits<double>::infinity();
        for (uint32_t v = 0; v < (all_paths[idx].size() - 1); v++) {
            i = all_paths[idx][v];
            j = all_paths[idx][v + 1];
            e = _model.graph.A[i][j].id;
            paths.back().push_back(e);
            vertices.back().push_back(i);
            if (v == (all_paths[idx].size() - 2)) {
                vertices.back().push_back(j);
            }
            if (smallest_amount > edges[e]) {
                smallest_amount = edges[e];
            }
        }
        ratios.push_back(smallest_amount);
    }
}

UIntMat_t Solution::findAllPaths(uint32_t s, uint32_t t, const DoubleVec_t& edges)
{
    UIntMat_t found_paths;
    UIntMat_t pathlings;
    pathlings.push_back(UIntVec_t());
    pathlings.back().push_back(s);
    uint32_t size = 1;

    std::set<uint32_t> Q1 = {s};
    std::set<uint32_t> Q2;
    uint32_t step_counter = 1;

    while (step_counter < _model.graph.nb_vertices) {
        // Add neighboring non-visited and reachable vertices
        for (uint32_t v : Q1) {
            if (v != t) { // do not continue from target
                for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
                    if ((_model.graph.A[v][i].id != -1) && (edges[_model.graph.A[v][i].id] != 0.0)) {
                        Q2.insert(i);
                    }
                }
            }
        }

        // Append new vertex to all paths
        for (uint32_t v : Q2) {
            for (uint32_t idx = 0; idx < size; idx++) {
                bool visited = false;
                for (uint32_t v_idx = 0; v_idx < pathlings[idx].size(); v_idx++) {
                    if (v == pathlings[idx][v_idx]) {
                        visited = true;
                        break;
                    }
                }

                if ((! visited) && (_model.graph.A[pathlings[idx].back()][v].id != -1) && (edges[_model.graph.A[pathlings[idx].back()][v].id] != 0.0)) {
                    pathlings.push_back(pathlings[idx]);
                    pathlings.back().push_back(v);
                    if (v == t) { // save path from s to t
                        found_paths.push_back(pathlings.back());
                    }
                }
            }
        }

        // Next step
        Q1 = Q2;
        Q2.clear();
        step_counter++;

        pathlings.erase(pathlings.begin(), pathlings.begin() + size);
        size = pathlings.size();
    }

    return found_paths;
}
