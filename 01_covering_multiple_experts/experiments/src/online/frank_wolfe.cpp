#include "frank_wolfe.h"

#include <iomanip>
#include <sstream>


FrankWolfe::FrankWolfe(const Config& config, BaseModel& model, LP_Solver& solver) :
    T(config.time_horizon),
    max_search_iter(config.max_search_iter),
    max_dist(config.max_distance),
    x(model.getNbVariables(), 0.0),
    x_prev(model.getNbVariables(), 0.0),
    v(model.getNbVariables(), 0.0),
    d(model.getNbVariables(), 0.0),
    temp(model.getNbVariables(), 0.0),
    _solver(solver),
    _model(model)
{}

const DoubleVec_t& FrankWolfe::solve(const DoubleVec_t& initial_solution)
{
    x = initial_solution;

    double eta = 0.0;
    double euclid_norm = 0.0;

    // Optimize through T steps
    for (uint32_t t = 0; t < T; t++) {
        // Calculate v
        v = _solver.solve(_model.getObjectiveValueDerivative(x, x_prev));

        // Calculate the distance
        euclid_norm = 0.0;
        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            d[i] = v[i] - x[i];
            euclid_norm += d[i] * d[i];
        }

        // If the distance between the solutions is less than the limit, terminate
        if (euclid_norm < max_dist) {
            break;
        }

        // Update eta
        eta = computeNewEta();

        // Update x
        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            x[i] = x[i] + eta * d[i];
            if (x[i] < 0) {
                x[i] = 0;
            }
        }

        // We don't update the solution anymore
        if (eta < max_dist) {
            break;
        }
    }

    // Round the solution
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        x[i] = round(x[i]);
        x_prev[i] = x[i];
    }

    return x;
}

double FrankWolfe::computeNewEta()
{
    double min_eta = 0.0;
    double max_eta = 1.0;
    double best_value = getObjectiveValue(max_eta);
    double current_value;
    double current_eta;

    for (uint32_t it = 0; it < max_search_iter; it++) {
        current_eta = min_eta + ((max_eta - min_eta) / 2.0);
        current_value = getObjectiveValue(current_eta);
        if (current_value < best_value) {
            best_value = current_value;
            max_eta = current_eta;
        } else {
            min_eta = current_eta;
        }
    }

    return current_eta;
}

double FrankWolfe::getObjectiveValue(double eta)
{
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        temp[i] = x[i] + eta * d[i];
    }

    return _model.getObjectiveValue(temp, x_prev);
}

double FrankWolfe::round(double value)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(5) << value;
    double y;
    ss >> y;
    return y;
}
