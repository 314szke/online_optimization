#pragma once

#include "config/config.h"
#include "model/model.h"
#include "model/online_model.h"
#include "types/local_types.h"


class GreedySolver {
public:
    GreedySolver(const Config& config, Model& model);
    const DoubleVec_t& solve();
private:
    void findInitialSolution(uint32_t s, uint32_t t);
    void transformSolution(uint32_t r);

    const Config& _config;
    Model& _model;
    OnlineModel online_model;

    uint32_t T;
    DoubleVec_t solution;

    uint32_t nb_cp_variables;
    DoubleVec_t cp_solution;
};
