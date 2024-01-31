#include "convex_model.h"

#include <cmath>


ConvexModel::ConvexModel(const OfflineModel& model, const Experts& experts) :
    _offline_model(model),
    _experts(experts),
    time(0),
    nb_constraints_per_time(model.getNbVariables() + 1),
    nb_revealed_constraints(0)
{
    use_box_constraints = true;

    nb_variables = _offline_model.getNbVariables() * experts.getNbExperts();
    nb_constraints = 2 * _offline_model.getNbVariables();
    is_minimization = _offline_model.isMinimization();

    c.resize(nb_variables, 0.0);
    b.resize(1);
    A.resize(1);

    // Constraint independent of time: the sum of weights is one
    b[time].resize(nb_constraints);
    A[time].resize(nb_constraints);
    for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
        b[time][i] = 1.0;
        A[time][i].resize(nb_variables);
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[time][i][v] = 0.0;
        }
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[time][i][(i + (k * _offline_model.getNbVariables()))] = 1.0;
        }
    }
    for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
        uint32_t j = i + _offline_model.getNbVariables();
        b[time][j] = -1.0;
        A[time][j].resize(nb_variables);
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[time][j][v] = 0.0;
        }
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[time][j][(i + (k * _offline_model.getNbVariables()))] = -1.0;
        }
    }
}

void ConvexModel::revealNextConstraint()
{
    // Add space for new constraints
    time++;
    nb_revealed_constraints = (time - 1);
    nb_constraints += nb_constraints_per_time;
    online_step++;

    b.resize((time + 1));
    A.resize((time + 1));

    // We have +1 constraint for the first constraint and +n constraint for the third constraint
    b[time].resize(nb_constraints_per_time);
    A[time].resize(nb_constraints_per_time);

    const DoubleVec_t& b_lp = _offline_model.getBound(0);
    b[time][0] = b_lp[nb_revealed_constraints];
    for (uint32_t j = 1; j < nb_constraints_per_time; j++) {
        b[time][j] = 0.0;
    }

    for (uint32_t j = 0; j < nb_constraints_per_time; j++) {
        A[time][j].resize(nb_variables);
        for (uint32_t v = 0; v < nb_variables; v++) {
            A[time][j][v] = 0.0;
        }
    }

    // Add new LP constraint satisfaction constraint
    const DoubleMat_t& A_lp = _offline_model.getCoefficient(0);
    const DoubleMat_t& s_hat = _experts.getTightSolutions(nb_revealed_constraints);

    for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
        for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
            A[time][0][(i + (k * _offline_model.getNbVariables()))] = A_lp[nb_revealed_constraints][i] * s_hat[k][i];
        }
    }

    // Add non-negative variable constraint
    const DoubleMat_t& s = _experts.getSolutions(nb_revealed_constraints);

    for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            A[time][(i + 1)][(i + (k * _offline_model.getNbVariables()))] = s[k][i];
        }
    }
}

double ConvexModel::getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const
{
    const DoubleVec_t& c_lp = _offline_model.getCost();
    const DoubleMat_t& s = _experts.getSolutions(nb_revealed_constraints);
    const DoubleMat_t& s_prev = _experts.getSolutions((nb_revealed_constraints - 1));
    const DoubleVec_t& avg = _experts.getAverageSolutions(nb_revealed_constraints);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions((nb_revealed_constraints - 1));

    double value = 0.0;
    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * _offline_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * _offline_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));
        value += c_lp[i] * ((x + avg[i]) * log_value);
        value -= c_lp[i] * x;
    }

    return value;
}

void ConvexModel::calculateObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev)
{
    const DoubleVec_t& c_lp = _offline_model.getCost();
    const DoubleMat_t& s = _experts.getSolutions(nb_revealed_constraints);
    const DoubleMat_t& s_prev = _experts.getSolutions((nb_revealed_constraints - 1));
    const DoubleVec_t& avg = _experts.getAverageSolutions(nb_revealed_constraints);
    const DoubleVec_t& avg_prev = _experts.getAverageSolutions((nb_revealed_constraints - 1));

    double x = 0.0;
    double x_prev = 0.0;
    double log_value = 0.0;

    for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
        x = 0.0;
        x_prev = 0.0;
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            x += s[k][i] * w[(i + (k * _offline_model.getNbVariables()))];
            x_prev += s_prev[k][i] * w_prev[(i + (k * _offline_model.getNbVariables()))];
        }
        log_value = std::log((x + avg[i]) / (x_prev + avg_prev[i]));
        for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
            c[(i + (k * _offline_model.getNbVariables()))] = c_lp[i] * s[k][i] * log_value;
        }
    }
}

uint32_t ConvexModel::getNbLPVariables() const
{
    return _offline_model.getNbVariables();
}
