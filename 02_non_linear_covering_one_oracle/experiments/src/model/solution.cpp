#include "solution.h"


Solution::Solution(const Model& model, const DoubleVec_t& edges, uint32_t s, uint32_t t) :
    _model(model)
{
    // Find all paths used by the CP solution
    BoolVec_t visited(_model.graph.nb_vertices, false);
    UIntMat_t all_paths = findPathFrom(s, visited);

    // Remove paths which do not end in t
    bool removed_a_path;
    do {
        removed_a_path = false;

        for (uint32_t idx = 0; idx < all_paths.size(); idx++) {
            if (all_paths[idx].back() != t) {
                all_paths.erase(all_paths.begin() + idx);
                removed_a_path = true;
                break;
            }
        }

    } while (removed_a_path);

    // Remove paths which have edges not included in the CP solution
    uint32_t i, j, e;
    do {
        removed_a_path = false;

        for (uint32_t idx = 0; idx < all_paths.size(); idx++) {
            for (uint32_t v = 0; v < (all_paths[idx].size() - 1); v++) {
                i = all_paths[idx][v];
                j = all_paths[idx][v + 1];
                e = _model.graph.A[i][j]->id;
                if (edges[e] == 0.0) {
                    all_paths.erase(all_paths.begin() + idx);
                    removed_a_path = true;
                    break;
                }
            }
            if (removed_a_path) {
                break;
            }
        }

    } while (removed_a_path);


    // Convert vertices to edges
    double amount;
    double counter;

    for (uint32_t idx = 0; idx < all_paths.size(); idx++) {
        paths.push_back(UIntVec_t());

        amount = 0.0;
        counter = 0.0;
        for (uint32_t v = 0; v < (all_paths[idx].size() - 1); v++) {
            i = all_paths[idx][v];
            j = all_paths[idx][v + 1];
            e = _model.graph.A[i][j]->id;
            paths.back().push_back(e);
            amount += edges[e];
            counter++;
        }
        ratios.push_back(amount / counter);
    }
}

UIntMat_t Solution::findPathFrom(uint32_t s, const BoolVec_t& visited)
{
    // There is always a path from s to s
    UIntMat_t all_paths;
    all_paths.push_back(UIntVec_t());
    all_paths.back().push_back(s);

    BoolVec_t local_visited = visited;
    local_visited[s] = true;

    for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
        if (_model.graph.A[s][i] && (! local_visited[i])) {

            UIntMat_t new_paths = findPathFrom(i, local_visited);
            for (uint32_t idx = 0; idx < new_paths.size(); idx++) {
                all_paths.push_back(UIntVec_t());
                all_paths.back().push_back(s);
                for (uint32_t v = 0; v < new_paths[idx].size();  v++) {
                    all_paths.back().push_back(new_paths[idx][v]);
                }
            }
        }
    }

    return all_paths;
}
