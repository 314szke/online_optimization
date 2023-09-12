#pragma once

#include <random>

#include "config/config.h"
#include "model/cp_model.h"
#include "model/model.h"
#include "oracle.hpp"
#include "types/local_types.h"


class Prediction{
public:
    typedef std::vector<Oracle> OracleVec_t;

    Prediction(const Config& config, Model& model, const CP_Model::SolutionVec_t& offline_paths);

    uint32_t getNbOracles() const;
    const Oracle& getOracle(uint32_t oracle_idx) const;
    void printFormattedSolution(uint32_t oracle_idx) const;
private:
    bool oracleIsNotUnique(const Oracle& oracle);
    void createPredictions(Oracle& oracle);

    Model& _model;
    const CP_Model::SolutionVec_t& _solution;

    OracleVec_t oracles;

    std::random_device seed;
    std::mt19937 random_engine;
};
