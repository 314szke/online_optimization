#include "frank_wolfe.h"

#include<iostream>
#include "visualization/print.hpp"


FrankWolfe::FrankWolfe(const Config& config, Model& model, const Model::RequestVec_t& requests) :
    T(config.time_horizon),
    max_search_iter(config.max_search_iter),
    max_dist(config.max_distance),
    eta(0),
    lp_solver(model, requests),
    _model(model),
    _requests(requests)
{
    x.resize(_requests.size());
    v.resize(_requests.size());
    d.resize(_requests.size());
    temp.resize(_requests.size());

    for (uint32_t r = 0; r < _requests.size(); r++) {
        x[r].resize(_model.getNbEdges());
        v[r].resize(_model.getNbEdges());
        d[r].resize(_model.getNbEdges());
        temp[r].resize(_model.getNbEdges());
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            x[r][e] = 1.0;
            v[r][e] = 0.0;
            d[r][e] = 0.0;
            temp[r][e] = 0.0;
        }
    }
}

DoubleMat_t& FrankWolfe::solve(const DoubleVec_t& extra_cost)
{
    // Find initial feasible solution
    for (uint32_t r = 0; r < _requests.size(); r++) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            x[r][e] = 1.0;
            v[r][e] = 0.0;
        }
    }

    lp_solver.solve(x, v, extra_cost);

    for (uint32_t r = 0; r < _requests.size(); r++) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            x[r][e] = v[r][e];
        }
    }

    // Optimize through T steps
    double obj_value = 0.0;
    for (uint32_t t = 0; t < T; t++) {
        // Calculate v
        obj_value = lp_solver.solve(x, v, extra_cost);

        // Calculate the distance
        double e_norm = 0.0;
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            for (uint32_t r = 0; r < _requests.size(); r++) {
                d[r][e] = v[r][e] - x[r][e];
                e_norm += d[r][e] * d[r][e];
            }
        }

        // If the distance between the solutions is 0, terminate
        if (e_norm  < max_dist) {
            return x;
        }

        // Update eta
        double min_eta = 0.0;
        double max_eta = 1.0;
        double best_value = getFunctionValue(max_eta);
        double current_value;

        for (uint32_t it = 0; it < (max_search_iter + t); it++) {
            eta = min_eta + ((max_eta - min_eta) / 2.0);
            current_value = getFunctionValue(eta);
            if (current_value < best_value) {
                best_value = current_value;
                max_eta = eta;
            } else {
                min_eta = eta;
            }
        }

        // Update x
        for (uint32_t r = 0; r < _requests.size(); r++) {
            for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
                x[r][e] = x[r][e] + eta * d[r][e];
            }
        }
    }

    // Round x
    for (uint32_t r = 0; r < _requests.size(); r++) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            if (x[r][e] < 0.001) {
                x[r][e] = 0.0;
            }
        }
    }
    return x;
}

double FrankWolfe::getFunctionValue(const double eta)
{
    for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
        for (uint32_t r = 0; r < _requests.size(); r++) {
            temp[r][e] = x[r][e] + eta * d[r][e];
        }
    }
    return _model.getObjectiveValue(temp);
}