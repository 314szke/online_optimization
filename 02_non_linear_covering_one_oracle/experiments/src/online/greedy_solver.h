#pragma once

#include "config/config.h"
#include "model/model.h"
#include "types/local_types.h"


class GreedySolver {
public:
    GreedySolver(Model& model, const Config& config);
    DoubleMat_t& solve(const Request& request);
private:
    const Config& _config;
    Model& _model;

    DoubleMat_t costs;
    DoubleMat_t solution;
    DoubleMat_t temp;
};
