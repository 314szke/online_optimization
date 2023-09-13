#include "greedy_solver.h"


#include "model/graph.h"
#include "model/solution.h"
#include "offline/frank_wolfe.h"


GreedySolver::GreedySolver(const Config& config, Model& model) :
    _config(config),
    _model(model),
    online_model(model),
    solution(online_model.getNbVariables(), 0.0),
    nb_cp_variables(model.graph.nb_edges + (model.graph.nb_edges * model.nb_requests)),
    cp_solution(nb_cp_variables, 0.0)
{}

const DoubleVec_t& GreedySolver::solve()
{
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        online_model.next();
        findInitialSolution(_model.requests[r].source, _model.requests[r].target);

        FrankWolfe fw(_config, online_model);
        solution = fw.solve(solution);

        transformSolution(r);
    }

    return cp_solution;
}

void GreedySolver::findInitialSolution(uint32_t s, uint32_t t)
{
    solution = DoubleVec_t(online_model.getNbVariables(), 0.0);
    Graph::Path_t path = _model.graph.getPath(s, t);
    for (uint32_t idx = 0; idx < path.size(); idx++) {
        solution[path[idx]] = 1.0;
    }
}

void GreedySolver::transformSolution(uint32_t r)
{
    uint32_t e, i, j;
    uint32_t best_path = 0;
    double best_objective = std::numeric_limits<double>::infinity();

    Solution formatted_solution = Solution(_model, solution, _model.requests[r].source, _model.requests[r].target);

    // Take the min cost path
    if (formatted_solution.paths.size() > 1) {
        for (uint32_t p = 0; p < formatted_solution.paths.size(); p++) {
            double cost = 0.0;
            for (uint32_t e_idx = 0; e_idx < formatted_solution.paths[p].size(); e_idx++) {
                e = formatted_solution.paths[p][e_idx];
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

    for (uint32_t e_idx = 0; e_idx < formatted_solution.paths[best_path].size(); e_idx++) {
        e = formatted_solution.paths[best_path][e_idx];
        cp_solution[(start + e)] = 1.0;
        cp_solution[e] += 1.0;
    }
}
