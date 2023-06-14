#pragma once

#include "config/config.h"
#include "lp_solver/lp_solver.h"
#include "types/local_types.h"


class FrankWolfe {
public:
    FrankWolfe(const Config& config, Model& model, const Model::RequestVec_t& requests);
    DoubleMat_t& solve(const DoubleVec_t& extra_cost);
private:
    double getFunctionValue(const double eta);

    uint32_t T;
    uint32_t max_search_iter;
    double max_dist;

    DoubleMat_t x;
    DoubleMat_t v;
    DoubleMat_t d;

    DoubleMat_t temp;

    double eta;

    LP_Solver lp_solver;
    Model& _model;
    const Model::RequestVec_t& _requests;
};
