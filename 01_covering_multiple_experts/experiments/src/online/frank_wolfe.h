#pragma once

#include "config/config.h"
#include "model/convex_model.h"
#include "model/experts.h"
#include "model/offline_model.h"
#include "offline/lp_solver.h"
#include "types/local_types.h"


class FrankWolfe {
public:
    FrankWolfe(const Config& config, ConvexModel& model, LP_Solver& solver);
    const DoubleVec_t& solve(const OfflineModel& off_model, const Experts& experts, uint32_t constr_limit);
private:
    double computeNewEta();
    double getObjectiveValue(double eta);
    void verifyFeasibility(const OfflineModel& off_model, const Experts& experts, uint32_t constr_limit);

    uint32_t T;
    uint32_t max_search_iter;
    double max_dist;

    DoubleVec_t x;
    DoubleVec_t x_prev;
    DoubleVec_t v;
    DoubleVec_t d;
    DoubleVec_t temp;

    LP_Solver& _solver;
    ConvexModel& _model;
};
