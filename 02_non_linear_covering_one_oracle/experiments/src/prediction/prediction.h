#pragma once

#include <random>

#include "config/config.h"
#include "model/model.h"
#include "types/local_types.h"


class Prediction {
public:
    Prediction(const Config& config, Model& model, const DoubleMat_t& offline_paths);
private:
    bool oracleIsNotUnique(double obj_value);
    void createPredictions(const Model::RequestVec_t& requests_copy);

    Model& _model;

    UIntMat_t predictions;
    DoubleVec_t objective_values;

    std::random_device rd;
    std::mt19937 random_engine;
};
