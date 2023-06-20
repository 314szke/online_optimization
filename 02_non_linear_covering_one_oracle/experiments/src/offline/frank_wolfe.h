#pragma once

#include "config/config.h"
#include "lp_solver.h"
#include "model/base_model.h"
#include "types/local_types.h"


class FrankWolfe {
public:
    FrankWolfe(const Config& config, BaseModel& model);
    const DoubleVec_t& solve(const DoubleVec_t& initial_solution);
private:
    double computeNewEta();
    double getObjectiveValue(double eta);

    uint32_t T;
    uint32_t max_search_iter;
    double max_dist;

    DoubleVec_t x;
    DoubleVec_t v;
    DoubleVec_t d;

    DoubleVec_t temp;

    BaseModel& _model;
    LP_Solver lp_solver;
};
