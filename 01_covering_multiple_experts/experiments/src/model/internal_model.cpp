#include "internal_model.h"

#include <cmath>


InternalModel::InternalModel(const Config& config, const OfflineModel& model, const Experts& experts) :
    online_model(model),
    _experts(experts),
    L(config.L)
{
    is_convex = online_model.isConvex();
    is_online = true;
    use_box_constraints = true;

    nb_variables = online_model.getNbVariables() * experts.getNbExperts();
    nb_objective_variables = nb_variables;

    nb_batches = online_model.getNbConstraintBatches();
    batch_size = online_model.getNbInitialConstraints() + online_model.getConstraintBatchSize() + online_model.getNbVariables();

    nb_initial_constraints = (2 * online_model.getNbVariables());
    nb_constraints = nb_initial_constraints + (nb_batches * batch_size);

    c.resize(nb_variables, 0.0);
    dc.resize(nb_variables, 0.0);
    e.resize(nb_variables, 1.0);

    initial_solution.resize(nb_variables, 0.0);
    // Choose the first expert
    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        initial_solution[i] = 1.0;
    }

    // Initial constraints
    b.resize(1);
    A.resize(1);
    b[0].resize(nb_initial_constraints, 0.0);
    A[0].resize(nb_initial_constraints);

    // Sum of weights is >= 1
    nb_revealed_constraints = 0;
    uint32_t j = 0;
    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        j = (nb_revealed_constraints + i);
        b[0][j] = 1.0;
        A[0][j].resize(nb_variables, 0.0);
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[0][j][(i + (k * online_model.getNbVariables()))] = 1.0;
        }
    }
    nb_revealed_constraints += online_model.getNbVariables();

    // Sum of weights is <= 1
    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        j = (nb_revealed_constraints + i);
        b[0][j] = -1.0;
        A[0][j].resize(nb_variables, 0.0);
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[0][j][(i + (k * online_model.getNbVariables()))] = -1.0;
        }
    }
    nb_revealed_constraints = nb_initial_constraints;
}

void InternalModel::revealNextConstraints()
{
    // Add space for new constraints
    time++;
    online_model.revealNextConstraints();

    b.resize(getNbSteps());
    A.resize(getNbSteps());

    b[time].resize(batch_size, 0.0);
    A[time].resize(batch_size);

    const DoubleVec_t& b_lp_0 = online_model.getBound(0);
    const DoubleMat_t& A_lp_0 = online_model.getCoefficient(0);
    const DoubleVec_t& b_lp = online_model.getBound(time);
    const DoubleMat_t& A_lp = online_model.getCoefficient(time);
    const DoubleMat_t& s_hat = _experts.getTightSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s = _experts.getSolutions(time - 1);

    // Initial constraints need to be updated
    for (uint32_t j = 0; j < online_model.getNbInitialConstraints(); j++) {
        b[time][j] = b_lp_0[j];
        A[time][j].resize(nb_variables, 0.0);
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
                A[time][j][(i + (k * online_model.getNbVariables()))] = A_lp_0[j][i] * s_hat[k][i];
            }
        }
    }

    // Constraints from the problem
    uint32_t j = 0;
    for (uint32_t idx = 0; idx < online_model.getConstraintBatchSize(); idx++) {
        j = (idx + online_model.getNbInitialConstraints());
        b[time][j] = b_lp[idx];
        A[time][j].resize(nb_variables, 0.0);

        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
                A[time][j][(i + (k * online_model.getNbVariables()))] = A_lp[idx][i] * s_hat[k][i];
            }
        }
    }

    // Non-negative variable constraints
    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        j = (i + online_model.getNbInitialConstraints() + online_model.getConstraintBatchSize());
        b[time][j] = 0.0;
        A[time][j].resize(nb_variables, 0.0);

        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[time][j][(i + (k * online_model.getNbVariables()))] = s[k][i];
        }
    }

    nb_revealed_constraints += batch_size;
}

double InternalModel::getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    if (is_convex) {
        return getConvexObjectiveValue(w, w_prev);
    }
    return getLinearObjectiveValue(w, w_prev);
}

double InternalModel::getLinearObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    const DoubleVec_t& c_lp = online_model.getCost();
    const DoubleMat_t& s = _experts.getSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s_prev = _experts.getSolutions(time - 2);
    const DoubleVec_t& avg = _experts.getAverageSolutions(time - 1);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions(time - 2);

    double value = 0.0;
    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * online_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * online_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));
        value += c_lp[i] * ((x + avg[i]) * log_value);
        value -= c_lp[i] * x;
    }

    return value;
}

double InternalModel::getConvexObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    const DoubleVec_t& c_lp = online_model.getCost();
    const DoubleVec_t& e_lp = online_model.getCostExponent();
    const DoubleMat_t& s = _experts.getSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s_prev = _experts.getSolutions(time - 2);
    const DoubleVec_t& avg = _experts.getAverageSolutions(time - 1);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions(time - 2);

    double value = 0.0;
    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;
    double df = 0.0;
    double subpart = 0.0;

    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * online_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * online_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));
        df = (c_lp[i] * e_lp[i]) * std::pow(x_prev, (e_lp[i] - 1));
        subpart = (x + avg[i] - (2 * x_prev) - (2 * avg_prev[i]));

        value += df * (((x + avg[i]) * log_value) - x);
        value += (L / 2.0) * subpart * (x + avg[i]) * log_value;
        value -= (L / 4.0) * std::pow(subpart, 2.0);
    }

    return value;
}

const DoubleVec_t& InternalModel::getObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    if (is_convex) {
        calculateConvexObjectiveValueDerivative(w, w_prev);
    } else {
        calculateLinearObjectiveValueDerivative(w, w_prev);
    }
    return c;
}

void InternalModel::calculateLinearObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    const DoubleVec_t& c_lp = online_model.getCost();
    const DoubleMat_t& s = _experts.getSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s_prev = _experts.getSolutions(time - 2);
    const DoubleVec_t& avg = _experts.getAverageSolutions(time - 1);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions(time - 2);

    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * online_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * online_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            c[(i + (k * online_model.getNbVariables()))] = c_lp[i] * s[k][i] * log_value;
        }
    }
}

void InternalModel::calculateConvexObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    const DoubleVec_t& c_lp = online_model.getCost();
    const DoubleVec_t& e_lp = online_model.getCostExponent();
    const DoubleMat_t& s = _experts.getSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s_prev = _experts.getSolutions(time - 2);
    const DoubleVec_t& avg = _experts.getAverageSolutions(time - 1);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions(time - 2);

    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;
    uint32_t idx = 0;
    double df = 0.0;

    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * online_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * online_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));

        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            idx = (i + (k * online_model.getNbVariables()));
            df = (c_lp[i] * e_lp[i]) * std::pow(x_prev, (e_lp[i] - 1));
            c[idx] = s[k][i] * (df + L * (x + avg[i] - x_prev - avg_prev[i])) * log_value;
        }
    }
}

const DoubleVec_t& InternalModel::getInitialSolution() const
{
    return initial_solution;
}

uint32_t InternalModel::getNbOriginalRevealedConstraints() const
{
    return online_model.getNbRevealedConstraints();
}
