#include "frank_wolfe.h"

#include <iomanip>
#include <sstream>


FrankWolfe::FrankWolfe(const Config& config, ConvexModel& model, LP_Solver& solver) :
    T(config.time_horizon),
    max_search_iter(config.max_search_iter),
    max_dist(config.max_distance),
    _solver(solver),
    _model(model)
{
    x.resize(_model.getNbVariables(), 1.0);
    x_prev.resize(_model.getNbVariables(), 0.0);
    v.resize(_model.getNbVariables(), 0.0);
    d.resize(_model.getNbVariables(), 0.0);
    temp.resize(_model.getNbVariables(), 0.0);
}

const DoubleVec_t& FrankWolfe::solve()
{
    // Set initial feasible solution
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        x[i] = 0.0;
    }
    // Choose the first expert
    for (uint32_t i = 0; i < _model.getNbLPVariables(); i++) {
        x[i] = 1.0;
    }

    double eta = 0.0;
    double euclid_norm = 0.0;

    // Optimize through T steps
    for (uint32_t t = 0; t < T; t++) {
        // Calculate v
        _model.calculateObjectiveValueDerivative(x, x_prev);
        v = _solver.solve();

        // Calculate the distance
        euclid_norm = 0.0;
        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            d[i] = v[i] - x[i];
            euclid_norm += d[i] * d[i];
        }

        // If the distance between the solutions is less than the limit, terminate
        if (euclid_norm < max_dist) {
            return x;
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
    }

    // Save x for objective value calculation
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
    ss << std::fixed << std::setprecision(2) << value;
    double y;
    ss >> y;
    return y;
}
