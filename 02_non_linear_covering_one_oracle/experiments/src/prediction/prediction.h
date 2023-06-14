#pragma once

#include<random>

#include "config/config.h"
#include "model/model.h"
#include "model/request.hpp"
#include "path_finding/CBFS.h"
#include "types/local_types.h"


class Prediction {
public:
    Prediction(Model& model, const DoubleMat_t& off_solution);
    const DoubleMat_t& createPredictionWithError(const double target_error_rate);
    double getErrorRate() const;
    const BoolVec_t& predict(const Request& request) const;
private:
    void createIntegralSolution(const DoubleMat_t& offline_solution);
    void initializeToIntegralSolution();
    void introduceErrors(const Request& request);

    Model& _model;
    CBFS cbfs;

    double pred_error;

    DoubleMat_t integral_solution;
    DoubleMat_t temp;
    BoolMat_t prediction;
    DoubleMat_t solution;

    BoolMat_t available_edges;
    std::random_device rd;
    std::mt19937 random_engine;
};
