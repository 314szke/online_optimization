#pragma once

#include "base/base_model.h"
#include "config/config.h"
#include "model/experts.h"
#include "model/offline_model.h"
#include "offline/lp_solver.h"
#include "types/local_types.h"


class FrankWolfe {
public:
    FrankWolfe(const Config& config, BaseModel& model, LP_Solver& solver);
    const DoubleVec_t& solve(const DoubleVec_t& initial_solution);
private:
    double computeNewEta();
    double getObjectiveValue(double eta);
    double round(double value);

    uint32_t T;
    uint32_t max_search_iter;
    double max_dist;

    DoubleVec_t x;
    DoubleVec_t x_prev;
    DoubleVec_t v;
    DoubleVec_t d;
    DoubleVec_t temp;

    LP_Solver& _solver;
    BaseModel& _model;
};
