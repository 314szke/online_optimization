#include "greedy_solver.h"


#include "model/graph.h"
#include "model/online_model.h"
#include "offline/frank_wolfe.h"
#include "visualization/print.hpp"


GreedySolver::GreedySolver(const Config& config, Model& model) :
    _config(config),
    _model(model),
    online_model(model),
    T(model.nb_requests),
    solution(online_model.getNbVariables(), 0.0),
    nb_cp_variables(model.graph.nb_edges + (model.graph.nb_edges * model.nb_requests)),
    cp_solution(nb_cp_variables, 0.0)
{}

const DoubleVec_t& GreedySolver::solve()
{
    for (uint32_t t = 0; t < T; t++) {
        online_model.next();
        findInitialSolution(_model.requests[t].source, _model.requests[t].target);

        FrankWolfe fw(_config, online_model);
        solution = fw.solve(solution);

        transformSolution(t);
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
    uint32_t start = _model.graph.nb_edges + (r * _model.graph.nb_edges);

    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        cp_solution[(start + e)] = solution[e];
        cp_solution[e] += solution[e];
    }
}
