#include "greedy_solver.h"

#include "lp_solver/lp_solver.h"
#include "offline/frank_wolfe.h"
#include "visualization/print.hpp"


GreedySolver::GreedySolver(Model& model) :
    _model(model)
{
    _config.time_horizon = 40;
    _config.max_search_iter = 10;
    _config.max_distance = 0.001;

    costs.resize(_model.getNbRequests());
    solution.resize(_model.getNbRequests());
    temp.push_back(DoubleVec_t(_model.getNbEdges(), 0.0));

    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        costs[r].resize(_model.getNbEdges());
        solution[r].resize(_model.getNbEdges());
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            costs[r][e] = 0.0;
            solution[r][e] = 0.0;
        }
    }
}

DoubleMat_t& GreedySolver::solve(const Request& request)
{
    // The LP solver expects a list of requests
    Model::RequestVec_t requests;
    requests.push_back(request);

    // Simulate the cost by an increase of 1 on the edges
    for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
        double sum = 0.0;
        for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
            sum += solution[r][e];
        }
        costs[request.id][e] = _model.getEdgeCost(e, sum);
    }

    // Calculate the min cost path for the request
    FrankWolfe fw(_config, _model, requests);
    temp = fw.solve(costs[request.id]);

    // Update only the current solution
    for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
        solution[request.id][e] = temp[0][e]; // LP only gets one request
    }

    return solution;
}
