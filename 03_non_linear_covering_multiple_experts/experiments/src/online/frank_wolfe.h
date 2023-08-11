#pragma once

#include "config/config.h"
#include "model/convex_model.h"
#include "model/experts.h"
#include "model/offline_model.h"
#include "online/lp_solver.h"
#include "types/local_types.h"


class FrankWolfe {
public:
    FrankWolfe(const Config& config, ConvexModel& model);
    const DoubleVec_t& solve();
private:
    double computeNewEta();
    double getObjectiveValue(double eta);

    const Config& _config;
    ConvexModel& _model;

    uint32_t T;
    uint32_t max_search_iter;
    double max_dist;

    DoubleVec_t x;
    DoubleVec_t x_prev;
    DoubleVec_t v;
    DoubleVec_t d;
    DoubleVec_t temp;
};
