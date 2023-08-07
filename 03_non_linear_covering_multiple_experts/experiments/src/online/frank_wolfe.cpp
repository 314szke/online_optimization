#include "frank_wolfe.h"

#include "visualization/print.hpp"


FrankWolfe::FrankWolfe(const Config& config, ConvexModel& model, LP_Solver& solver) :
    T(config.time_horizon),
    max_search_iter(config.max_search_iter),
    max_dist(config.max_distance),
    _solver(solver),
    _model(model)
{
    x.resize(_model.getNbVariables());
    x_prev.resize(_model.getNbVariables());
    v.resize(_model.getNbVariables());
    d.resize(_model.getNbVariables());
    temp.resize(_model.getNbVariables());

    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        x[i] = 1.0;
        x_prev[i] = 0.0;
        v[i] = 0.0;
        d[i] = 0.0;
        temp[i] = 0.0;
    }
}

const DoubleVec_t& FrankWolfe::solve(const OfflineModel& off_model, const Experts& experts, uint32_t constr_limit)
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
        verifyFeasibility(off_model, experts, constr_limit);

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

    // Round tiny x
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
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

void FrankWolfe::verifyFeasibility(const OfflineModel& off_model, const Experts& experts, uint32_t constr_limit)
{
    const DoubleVec_t& b = off_model.getBound(0);

    for (uint32_t t = 0; t < constr_limit; t++) {
        const DoubleMat_t& A_hat = _model.getCoefficient(t+1);
        double value = 0.0;
        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            value += A_hat[0][i] * v[i];
        }
        if (value < b[t]) {

            if ((b[t] - value) > 0.001) {
                std::stringstream message;
                message << "ERROR: The LP really gave an infeasible solution for constraint " << (constr_limit - 1) << " !" << std::endl;
                throw std::runtime_error(message.str());
            }
        }
    }

    const DoubleMat_t& A = off_model.getCoefficient(0);

    for (uint32_t t = 0; t < constr_limit; t++) {
        const DoubleMat_t& s_hat = experts.getSolutions(t);
        double value = 0.0;
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            double temp_x = 0.0;
            for (uint32_t k = 0; k < experts.getNbExperts(); k++) {
                temp_x += s_hat[k][i] * v[(i + (k * off_model.getNbVariables()))];
            }
            value += A[t][i] * temp_x;
        }
        if (value < b[t]) {
            if ((b[t] - value) > 0.001) {
                std::stringstream message;
                message << "ERROR: The LP gave an infeasible solution for constraint " << (constr_limit - 1) << " !" << std::endl;
                throw std::runtime_error(message.str());
            }
        }
    }


    for (uint32_t t = 0; t < constr_limit; t++) {
        const DoubleMat_t& s = experts.getSolutions(t);
        double value = 0.0;
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            double temp_x = 0.0;
            for (uint32_t k = 0; k < experts.getNbExperts(); k++) {
                temp_x += s[k][i] * v[(i + (k * off_model.getNbVariables()))];
            }
            value += A[t][i] * temp_x;
        }
        if (value < b[t]) {
            if ((b[t] - value) > 0.001) {
                std::stringstream message;
                message << "ERROR: The tight solutions satisfy the constraints, but not solutions at constraint " << (constr_limit - 1) << " !" << std::endl;
                throw std::runtime_error(message.str());
            }
        }
    }
}
