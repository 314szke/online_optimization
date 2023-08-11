#include "convex_model.h"

#include <cmath>


ConvexModel::ConvexModel(const OfflineModel& model, const Experts& experts) :
    offline_model(model),
    _experts(experts),
    p(std::log(model.getNbMachines())),
    L(p - 1.0),
    time(-1),
    x(offline_model.getNbVariables(), 0.0),
    x_prev(offline_model.getNbVariables(), 0.0)
{
    nb_machines = offline_model.getNbMachines() ;
    nb_jobs = offline_model.getNbJobs();

    nb_variables = nb_machines * experts.getNbExperts();
    nb_constraints = (2 * offline_model.getNbVariables()) + 1;

    c.resize(nb_variables, 0.0);
    b.resize(2);
    A.resize(2);

    b[0].resize(offline_model.getNbVariables());
    b[1].resize(offline_model.getNbVariables() + 1);
    A[0].resize(offline_model.getNbVariables());
    A[1].resize(offline_model.getNbVariables() + 1);

    for (uint32_t j = 0; j < offline_model.getNbVariables(); j++) {
        b[0][j] = 1.0;
        b[1][j] = 0.0;

        A[0][j].resize(nb_variables);
        A[1][j].resize(nb_variables);

        for (uint32_t v = 0; v < nb_variables; v++) {
            A[0][j][v] = 0.0;
        }
        // Constraint independent of time: the sum of weights is one
        for (uint32_t i = 0; i < nb_machines; i++) {
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                A[0][j][getLocalId(i,k)] = 1.0;
            }
        }
    }

    // Each step has n + 1 constraints
    b[1][offline_model.getNbVariables()] = 1.0;
    A[1][offline_model.getNbVariables()].resize(nb_variables);
}

void ConvexModel::revealNextConstraint()
{
    // Aligned with the index of the constraints of the offline model
    time++;

    // Clear previous coefficients
    for (uint32_t i = 0; i < (offline_model.getNbVariables() + 1); i++) {
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[1][i][v] = 0.0;
        }
    }

    const DoubleMat_t& A_offline = offline_model.getCoefficient();
    const DoubleMat_t& s = _experts.getSolutions(time);

    // Add non-negative variable constraint
    for (uint32_t j = 0; j < offline_model.getNbVariables(); j++) {
        for (uint32_t i = 0; i < nb_machines; i++) {
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                A[1][j][getLocalId(i,k)] = s[k][getId(i,j)];
            }
        }
    }

    // Add new LP constraint satisfaction constraint
    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[1][offline_model.getNbVariables()][getLocalId(i,k)] = A_offline[time][getId(i,time)] * s[k][getId(i,time)];
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

    uint32_t idx;

    // Calculate x and x_prev
    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            idx = getId(i,time);
            x[idx] += s[k][idx] * w[getLocalId(i,k)];
            x_prev[idx] += s_prev[k][idx] * w_prev[getLocalId(i,k)];
        }
    }

    double value = 0.0;
    double d_i_f_value = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t j = 0; j < nb_jobs; j++) {
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

    uint32_t idx;

    // Calculate x and x_prev
    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            idx = getId(i,time);
            x[idx] += s[k][idx] * w[getLocalId(i,k)];
            x_prev[idx] += s_prev[k][idx] * w_prev[getLocalId(i,k)];
        }
    }

    double d_i_f_value = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t j = 0; j < nb_jobs; j++) {
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
                c[getLocalId(i,k)] = s[k][idx] * (d_i_f_value + (L * x[idx])) * log_value;
            }
        }
    }
}

void ConvexModel::updateSolution(const DoubleVec_t& solution)
{
    for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
        x[i] = solution[i];
        x_prev[i] = solution[i];
    }
}

uint32_t ConvexModel::getLocalId(uint32_t i, uint32_t k) const
{
    // Each expert suggests a fraction for each machine to assign the current job
    return (i + (k * nb_machines));
}

double ConvexModel::f(const DoubleVec_t& x) const
{
    const DoubleVec_t& c_offline = offline_model.getCost();

    double loads_p_norm = 0.0;
    double load;
    uint32_t idx;

    for (uint32_t i = 0; i < nb_machines; i++) {
        load = 0.0;
        for (uint32_t j = 0; j < nb_jobs; j++) {
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
    for (uint32_t j = 0; j < nb_jobs; j++) {
        local_idx = getId(machine_i, j);
        load_i += c_offline[local_idx] * x[local_idx];
    }

    double loads_pow_p = 0.0;
    double load = 0.0;
    for (uint32_t i = 0; i < nb_machines; i++) {
        load = 0.0;
        for (uint32_t j = 0; j < nb_jobs; j++) {
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
