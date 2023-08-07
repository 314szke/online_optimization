#include "convex_model.h"

#include <cmath>


ConvexModel::ConvexModel(const OfflineModel& model, const Experts& experts) :
    offline_model(model),
    _experts(experts),
    p(std::log(model.getNbMachines())),
    L(p - 1.0),
    time(-1),
    nb_constraints_per_time(model.getNbVariables() + 1)
{
    nb_variables = offline_model.getNbVariables() * experts.getNbExperts();
    nb_constraints = offline_model.getNbVariables();

    c.resize(nb_variables, 0.0);
    b.resize(1);
    A.resize(1);

    // Constraint independent of time: the sum of weights is one
    b[0].resize(nb_constraints);
    A[0].resize(nb_constraints);
    for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
        b[0][i] = 1.0;

        A[0][i].resize(nb_variables);
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[0][i][v] = 0.0;
        }
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[0][i][getLocalId(i,k)] = 1.0;
        }
    }
}

void ConvexModel::revealNextConstraint()
{
    // Aligned with the index of the constraints of the offline model
    time++;

    // +1 constraint for the first constraint of the convex program
    // +n constraint for the third constraint of the convex program
    nb_constraints += nb_constraints_per_time;

    // set the bounds
    b.push_back(DoubleVec_t(nb_constraints_per_time, 0.0));
    b[(time + 1)][0] = 1.0;

    // set the coefficients
    A.resize((A.size() + 1));
    A[(time + 1)].resize(nb_constraints_per_time);

    for (uint32_t j = 0; j < nb_constraints_per_time; j++) {
        A[(time + 1)][j].resize(nb_variables);
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[(time + 1)][j][v] = 0.0;
        }
    }

    // Add new LP constraint satisfaction constraint
    const DoubleMat_t& A_offline = offline_model.getCoefficient();
    const DoubleMat_t& s = _experts.getSolutions(time);

    for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
        for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
            A[(time + 1)][0][getLocalId(i,k)] = A_offline[time][i] * s[k][i];
        }
    }

    // Add non-negative variable constraint
    for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            // (i+1)th constraint within batch
            A[(time + 1)][(i + 1)][getLocalId(i,k)] = s[k][i];
        }
    }
}

double ConvexModel::getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    const DoubleMat_t& s = _experts.getSolutions(time);
    const DoubleMat_t& s_prev = _experts.getSolutions((time - 1));
    const DoubleVec_t& avg = _experts.getAverageSolutions(time);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions((time - 1));

    DoubleVec_t x(offline_model.getNbVariables(), 0.0);
    DoubleVec_t x_prev(offline_model.getNbVariables(), 0.0);

    // Calculate x and x_prev
    for (uint32_t idx = 0; idx < offline_model.getNbVariables(); idx++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x[idx] += s[k][idx] * w[getLocalId(idx,k)];
            x_prev[idx] += s_prev[k][idx] * w_prev[getLocalId(idx,k)];
        }
    }

    double value = 0.0;
    double d_i_f_value = 0.0;
    double log_value = 0.0;
    uint32_t idx;

    for (uint32_t i = 0; i < offline_model.getNbMachines(); i++) {
        for (uint32_t j = 0; j < offline_model.getNbJobs(); j++) {
            idx = getId(i,j);

            // Calculate the log value
            if (((x[idx] + avg[idx]) == 0) || ((x_prev[idx] + avg_prev[idx]) == 0)) {
                log_value = 0;
            } else {
                log_value = std::log((x[idx] + avg[idx]) / (x_prev[idx] + avg_prev[idx]));
            }

            d_i_f_value = d_i_f(x_prev, i, idx);

            // Calculate the objective function value
            value += d_i_f_value * ((x[idx] + avg[idx]) * log_value);
            value -= d_i_f_value * x[idx];
            value += (L / 2.0) * (x[idx] - avg[idx]) * (x[idx] + avg[idx]) * log_value;
            value -= (L / 4.0) * (x[idx] - avg[idx]) * (x[idx] - avg[idx]);
        }
    }

    return value;
}

void ConvexModel::calculateObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    const DoubleMat_t& s = _experts.getSolutions(time);
    const DoubleMat_t& s_prev = _experts.getSolutions((time - 1));
    const DoubleVec_t& avg = _experts.getAverageSolutions(time);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions((time - 1));

    DoubleVec_t x(offline_model.getNbVariables(), 0.0);
    DoubleVec_t x_prev(offline_model.getNbVariables(), 0.0);

    // Calculate x and x_prev
    for (uint32_t idx = 0; idx < offline_model.getNbVariables(); idx++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x[idx] += s[k][idx] * w[getLocalId(idx,k)];
            x_prev[idx] += s_prev[k][idx] * w_prev[getLocalId(idx,k)];
        }
    }

    double d_i_f_value = 0.0;
    double log_value = 0.0;
    uint32_t idx;

    for (uint32_t i = 0; i < offline_model.getNbMachines(); i++) {
        for (uint32_t j = 0; j < offline_model.getNbJobs(); j++) {
            idx = getId(i,j);

            // Calculate the log value
            if (((x[idx] + avg[idx]) == 0) || ((x_prev[idx] + avg_prev[idx]) == 0)) {
                log_value = 0;
            } else {
                log_value = std::log((x[idx] + avg[idx]) / (x_prev[idx] + avg_prev[idx]));
            }

            d_i_f_value = d_i_f(x_prev, i, idx);

            // Calculate the objective function derivative
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                c[getLocalId(idx,k)] = s[k][idx] * (d_i_f_value + (L * x[idx])) * log_value;
            }
        }
    }
}

uint32_t ConvexModel::getNbLPVariables() const
{
    return offline_model.getNbVariables();
}

uint32_t ConvexModel::getLocalId(uint32_t i, uint32_t k) const
{
    // Convert Offline Model variable indices and Expert indices to one index
    return (i + (k * offline_model.getNbVariables()));
}

double ConvexModel::f(const DoubleVec_t& x) const
{
    const DoubleVec_t& c_offline = offline_model.getCost();

    double loads_p_norm = 0.0;
    double load;
    uint32_t idx;

    for (uint32_t i = 0; i < offline_model.getNbMachines(); i++) {
        load = 0.0;
        for (uint32_t j = 0; j < offline_model.getNbJobs(); j++) {
            idx = getId(i,j);
            load += c_offline[idx] + x[idx];
        }
        loads_p_norm += std::pow(load, p);
    }
    loads_p_norm = std::pow(loads_p_norm, (1.0 / p));

    return std::pow(loads_p_norm, 2.0);
}

double ConvexModel::d_i_f(const DoubleVec_t& x, uint32_t machine_i, uint32_t var_i) const
{
    const DoubleVec_t& c_offline = offline_model.getCost();

    double load_i = 0.0;
    uint32_t local_idx;
    for (uint32_t j = 0; j < offline_model.getNbJobs(); j++) {
        local_idx = getId(machine_i, j);
        load_i += c_offline[local_idx] * x[local_idx];
    }

    double loads_pow_p = 0.0;
    double load = 0.0;
    for (uint32_t i = 0; i < offline_model.getNbMachines(); i++) {
        load = 0.0;
        for (uint32_t j = 0; j < offline_model.getNbJobs(); j++) {
            local_idx = getId(i,j);
            load += c_offline[local_idx] * x[local_idx];
        }
        loads_pow_p += std::pow(load, p);
    }

    double nominator = c_offline[var_i] * std::pow(load_i, (p - 1.0));
    double denominator = std::pow(loads_pow_p, ((p - 1.0) / p));
    double norm_derivative = nominator / denominator;
    if (nominator == 0 || denominator == 0) {
        norm_derivative = 0.0;
    }

    return 2 * f(x) * norm_derivative;
}
