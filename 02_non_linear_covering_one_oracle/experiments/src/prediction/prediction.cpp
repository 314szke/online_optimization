#include "prediction.h"

#include <algorithm>
#include <limits>
#include <numeric>

#include "visualization/print.hpp"


Prediction::Prediction(Model& model, const DoubleVec_t& off_solution) :
    _model(model),
    cbfs(model),
    pred_error(0.0),
    random_engine(rd())
{

}

double Prediction::getErrorRate() const
{
    return pred_error;
}

const BoolVec_t& Prediction::predict(const Request& request) const
{
    return prediction[request.id];
}

void Prediction::createIntegralSolution(const DoubleMat_t& offline_solution)
{

}

const DoubleMat_t& Prediction::createPredictionWithError(const double target_error_rate)
{
    return solution;
}

void Prediction::initializeToIntegralSolution()
{

}

void Prediction::introduceErrors(const Request& request)
{

}
