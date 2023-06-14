#pragma once

#include <random>

#include "config/config.h"
#include "model/model.h"
#include "path_finding/CBFS.h"
#include "types/local_types.h"


class Solver {
public:
    Solver(Config& config, Model& model);
    DoubleMat_t& solve(const BoolVec_t& pred_x, const DoubleVec_t& greedy, const Request& request);
private:
    bool constraintIsSatisfied(const uint32_t s, const uint32_t t);
    double getDeltaF(const uint32_t e);

    double eps;
    uint32_t d;
    double L;
    double eta;
    double R;
    uint32_t c;

    Model& _model;
    CBFS cbfs;

    DoubleMat_t x;
    DoubleMat_t B;
    BoolMat_t marker;

    DoubleMat_t temp;
    std::random_device rd;
    std::mt19937 random_engine;
    std::uniform_real_distribution<double> uni_dist;
};
