#include "solver.h"

#include <limits>

#include "model/solution.h"


Solver::Solver(const Config& config, Model& model):
    _config(config),
    _model(model),
    edges(model.graph.nb_edges),
    wait_for_new_edge(false),
    nb_cp_variables(model.graph.nb_edges + (model.graph.nb_edges * model.nb_requests)),
    cp_solution(nb_cp_variables, 0.0),
    random_engine(config.random_seed),
    uni_dist(0.0, 1.0),
    random_set(_model.graph.nb_edges),
    random_numbers(1000000), // 1 million
    random_idx(0)
{
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        edges[e] = e;
    }

    for (uint32_t idx = 0; idx < random_numbers.size(); idx++) {
        random_numbers[idx] = uni_dist(random_engine);
    }
}

const DoubleVec_t& Solver::solve(const Oracle& oracle)
{
    // Initialize
    for (uint32_t idx = 0; idx < nb_cp_variables; idx++) {
        cp_solution[idx] = 0.0;
    }
    wait_for_new_edge = false;
    random_idx = 0;

    uint32_t e;
    double delta_e_F, lambda_delta_e_F;
    double increasing_rate;

    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        x = DoubleVec_t(_model.graph.nb_edges, 0.0);
        B = DoubleVec_t(_model.graph.nb_edges, 0.0);

        while (! pathExists(_model.requests[r].source, _model.requests[r].target)) {
            std::shuffle(edges.begin(), edges.end(), random_engine);
            for (uint32_t e_idx = 0; e_idx < _model.graph.nb_edges; e_idx++) {
                e = edges[e_idx];

                if (x[e] < 1.0) {
                    delta_e_F = getDeltaF(e);
                    lambda_delta_e_F = (1.0 / _config.lambda) * delta_e_F;
                    if (B[e] < lambda_delta_e_F) {
                        B[e] = lambda_delta_e_F;
                    }

                    increasing_rate = (x[e] / (_config.lambda * B[e]));
                    increasing_rate += (_config.eta / (_config.lambda * B[e] * _config.dimension));
                    if (oracle.predictions[r][e] == 1.0) {
                        increasing_rate += (1 - _config.eta) / (delta_e_F * oracle.dimensions[r]);
                    }

                    x[e] += increasing_rate * _config.DT;
                    if (x[e] >= (1.0 - _config.epsilon)) {
                        x[e] = 1.0;
                        wait_for_new_edge = false;
                        if (pathExists(_model.requests[r].source, _model.requests[r].target)) {
                            break;
                        }
                    }
                }
            }
        }
        transformSolution(r);
    }

    return cp_solution;
}

bool Solver::pathExists(uint32_t s, uint32_t t)
{
    if (wait_for_new_edge) {
        return false;
    }

    BoolVec_t visited(_model.graph.nb_vertices, false);
    visited[s] = true;

    UIntVec_t Q;
    Q.push_back(s);
    uint32_t idx = 0;

    uint32_t v;
    while (idx < Q.size()) {
        v = Q[idx];
        idx++;

        if (v == t) {
            return true;
        }

        for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
            // Edge exists, vertex i not visited and edge picked by the algo
            if ((_model.graph.A[v][i].id != -1) && (! visited[i]) && (x[_model.graph.A[v][i].id] == 1.0)) {
                visited[i] = true;
                Q.push_back(i);
            }
        }
    }

    wait_for_new_edge = true;
    return false;
}

double Solver::getDeltaF(uint32_t edge)
{
    double sum = 0.0;
    double random_number;
    uint32_t i,j;

    for (uint32_t it = 0; it < _config.random_iteration; it++) {
        for (uint32_t set_idx = 0; set_idx < _model.graph.nb_edges; set_idx++) {
            random_set[set_idx] = 0;
        }
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            if (e != edge) {
                random_number = getRandomNumber();
                if (random_number < x[e]) {
                    random_set[e] = 1;
                }
            }
        }

        // Cost with x[edge]
        double cost_with_edge = 0.0;
        random_set[edge] = 1;
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            i = _model.graph.ID[e].i;
            j = _model.graph.ID[e].j;
            cost_with_edge += _model.graph.A[i][j].getCost(cp_solution[e] + random_set[e]);
        }

        // Cost without x[edge]
        double cost = 0.0;
        random_set[edge] = 0;
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            i = _model.graph.ID[e].i;
            j = _model.graph.ID[e].j;
            cost += _model.graph.A[i][j].getCost(cp_solution[e] + random_set[e]);
        }

        sum += (cost_with_edge - cost);
    }

    return (sum / _config.random_iteration);
}

double Solver::getRandomNumber()
{
    random_idx++;
    if (random_idx > random_numbers.size()) {
        random_idx = 0;
    }
    return random_numbers[random_idx];
}

void Solver::transformSolution(uint32_t r)
{
    // Remove edges which were not selected
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        if (x[e] != 1.0) {
            x[e] = 0.0;
        }
    }

    Solution solution = Solution(_model, x, _model.requests[r].source, _model.requests[r].target);

    // Minimum cost path approach

    uint32_t e, i, j;
    uint32_t best_path = 0;
    double best_objective = std::numeric_limits<double>::infinity();

    // Take the min cost path
    if (solution.paths.size() > 1) {
        for (uint32_t p = 0; p < solution.paths.size(); p++) {
            double cost = 0.0;
            for (uint32_t e_idx = 0; e_idx < solution.paths[p].size(); e_idx++) {
                e = solution.paths[p][e_idx];
                i = _model.graph.ID[e].i;
                j = _model.graph.ID[e].j;
                cost += _model.graph.A[i][j].getCost(cp_solution[e] + 1);
            }
            if (cost < best_objective) {
                best_objective = cost;
                best_path = p;
            }
        }
    }

    uint32_t start = _model.graph.nb_edges + (r * _model.graph.nb_edges);

    for (uint32_t e_idx = 0; e_idx < solution.paths[best_path].size(); e_idx++) {
        e = solution.paths[best_path][e_idx];
        cp_solution[(start + e)] = 1.0;
        cp_solution[e] += 1.0;
    }


/*  // Fractional solution approach
    uint32_t e;
    double ratio = 1.0 / double(solution.paths.size());
    uint32_t start = _model.graph.nb_edges + (r * _model.graph.nb_edges);

    for (uint32_t p = 0; p < solution.paths.size(); p++) {
        for (uint32_t e_idx = 0; e_idx < solution.paths[p].size(); e_idx++) {
            e = solution.paths[p][e_idx];
            cp_solution[(start + e)] = ratio;
            cp_solution[e] += ratio;
        }
    }
*/
}
