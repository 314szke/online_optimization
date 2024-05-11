#include "convex_model.h"

#include <cmath>


ConvexModel::ConvexModel(const OfflineModel& model, const Experts& experts) :
    online_model(model),
    _experts(experts)
{
    is_convex = online_model.isConvex();
    is_online = true;
    use_box_constraints = true;

    nb_variables = online_model.getNbVariables() * experts.getNbExperts();
    nb_objective_variables = nb_variables;

    nb_batches = online_model.getNbConstraintBatches();
    batch_size = online_model.getConstraintBatchSize() + online_model.getNbVariables();

    nb_initial_constraints = online_model.getNbInitialConstraints() + (2 * online_model.getNbVariables());
    nb_constraints = nb_initial_constraints + (nb_batches * batch_size);

    c.resize(nb_variables, 0.0);

    // Initial constraints
    b.resize(1);
    A.resize(1);
    b[0].resize(nb_initial_constraints, 0.0);
    A[0].resize(nb_initial_constraints);

    const DoubleVec_t& b_lp = online_model.getBound(0);
    const DoubleMat_t& A_lp = online_model.getCoefficient(0);

    for (uint32_t j = 0; j < online_model.getNbInitialConstraints(); j++) {
        b[0][j] = b_lp[j];
        A[0][j].resize(nb_variables, 0.0);
        for (uint32_t i = 0; i < nb_variables; i++) {
            A[0][j][i] = A_lp[j][i];
        }
    }
    nb_revealed_constraints = online_model.getNbInitialConstraints();

    // Sum of weights is >= 1
    uint32_t j;
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

void ConvexModel::revealNextConstraints()
{
    // Add space for new constraints
    time++;
    online_model.revealNextConstraints();

    b.resize(getNbSteps());
    A.resize(getNbSteps());

    b[time].resize(batch_size, 0.0);
    A[time].resize(batch_size);

    const DoubleVec_t& b_lp = online_model.getBound(time);
    const DoubleMat_t& A_lp = online_model.getCoefficient(time);
    const DoubleMat_t& s_hat = _experts.getTightSolutions(time - 1); // time = 0 is reserved for initial constraints
    const DoubleMat_t& s = _experts.getSolutions(time - 1);

    // Constraints from the problem
    for (uint32_t j = 0; j < online_model.getConstraintBatchSize(); j++) {
        b[time][j] = b_lp[j];
        A[time][j].resize(nb_variables, 0.0);

        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
                A[time][j][(i + (k * online_model.getNbVariables()))] = A_lp[j][i] * s_hat[k][i];
            }
        }
    }

    // Non-negative variable constraints
    uint32_t j;
    for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
        j = online_model.getConstraintBatchSize() + i;
        b[time][j] = 0.0;
        A[time][j].resize(nb_variables, 0.0);

        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[time][j][(i + (k * online_model.getNbVariables()))] = s[k][i];
        }
    }

    nb_revealed_constraints += batch_size;
}

double ConvexModel::getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    if (is_convex) {
        return getConvexObjectiveValue(w, w_prev);
    }
    return getLinearObjectiveValue(w, w_prev);
}

double ConvexModel::getLinearObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
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

double ConvexModel::getConvexObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    return 0;
}

void ConvexModel::calculateObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    if (is_convex) {
        calculateConvexObjectiveValueDerivative(w, w_prev);
    } else {
        calculateLinearObjectiveValueDerivative(w, w_prev);
    }
}

void ConvexModel::calculateLinearObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
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

void ConvexModel::calculateConvexObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{

}

uint32_t ConvexModel::getNbLPVariables() const
{
    return online_model.getNbVariables();
}

uint32_t ConvexModel::getNbLPRevealedConstraints() const
{
    return online_model.getNbRevealedConstraints();
}
