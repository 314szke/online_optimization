#pragma once

#include <random>

#include "config/config.h"
#include "model/cp_model.h"
#include "model/model.h"
#include "types/local_types.h"


class Prediction{
public:
    Prediction(const Config& config, Model& model, const CP_Model::SolutionVec_t& offline_paths);
    const UIntMat_t& getPredictionsOfOracle(uint32_t oracle_idx) const;
    double getObjectiveValueOfOracle(uint32_t oracle_idx) const;
    void printFormattedSolution(uint32_t oracle_idx) const;
private:
    bool oracleIsNotUnique(uint32_t oracle_idx);
    void createPredictions(uint32_t oracle_idx);

    Model& _model;
    const CP_Model::SolutionVec_t& _solution;

    UIntMatVec_t predictions;
    DoubleVec_t objective_values;

    std::random_device seed;
    std::mt19937 random_engine;
};
