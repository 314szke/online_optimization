#pragma once

#include <random>

#include "config/config.h"
#include "model/model.h"
#include "prediction/oracle.hpp"
#include "types/local_types.h"

#include "random_store.h"


class Solver {
public:
    Solver(const Config& config, const RandomStore& random_store, const Model& model);
    const DoubleVec_t& solve(const Oracle& oracle);
private:
    bool pathExists(uint32_t s, uint32_t t);
    void transformSolution(uint32_t r);
    double getDeltaF(uint32_t edge);
    double getRandomNumber();

    const Config& _config;
    const RandomStore& _random_store;
    const Model& _model;

    DoubleVec_t x;
    DoubleVec_t B;

    UIntVec_t edges;
    bool wait_for_new_edge;

    uint32_t nb_cp_variables;
    DoubleVec_t cp_solution;

    std::mt19937 random_engine;
    UIntVec_t random_set;
    uint32_t random_idx;
};
