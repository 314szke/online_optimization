#include "base_model.h"

#include <math.h>
#include <sstream>


BaseModel::BaseModel() :
    is_convex(false),
    is_online(false),
    use_box_constraints(false),
    nb_variables(0),
    nb_constraints(0),
    nb_objective_variables(0),
    nb_initial_constraints(0),
    nb_batches(0),
    batch_size(1),
    time(0),
    nb_revealed_constraints(0)
{}

bool BaseModel::isConvex() const
{
    return is_convex;
}

bool BaseModel::isOnline() const
{
    return is_online;
}

bool BaseModel::useBoxConstraints() const
{
    return use_box_constraints;
}

uint32_t BaseModel::getNbVariables() const
{
    return nb_variables;
}

uint32_t BaseModel::getNbObjectiveVariables() const
{
    return nb_objective_variables;
}

uint32_t BaseModel::getNbConstraints() const
{
    return nb_constraints;
}

uint32_t BaseModel::getNbInitialConstraints() const
{
    return nb_initial_constraints;
}

uint32_t BaseModel::getNbRevealedConstraints() const
{
    return nb_revealed_constraints;
}

uint32_t BaseModel::getNbConstraintBatches() const
{
    return nb_batches;
}

uint32_t BaseModel::getConstraintBatchSize() const
{
    return batch_size;
}

uint32_t BaseModel::getNbConstraintsAt(const uint32_t current_time) const
{
    if (current_time >= b.size()) {
        return 0;
    }
    return b[current_time].size();
}

uint32_t BaseModel::getNbSteps() const
{
    return (time + 1);
}

const DoubleVec_t& BaseModel::getCost() const
{
    return c;
}

const DoubleVec_t& BaseModel::getCostExponent() const
{
    return e;
}

const DoubleVec_t& BaseModel::getBound(const uint32_t current_time) const
{
    if (current_time >= b.size()) {
        return empty_bound;
    }
    return b[current_time];
}

const DoubleMat_t& BaseModel::getCoefficient(const uint32_t current_time) const
{
    if (current_time >= A.size()) {
        return empty_coefficient;
    }
    return A[current_time];
}

double BaseModel::getObjectiveValue(const DoubleVec_t& x) const
{
    double value = 0.0;
    for (uint32_t i = 0; i < nb_variables; i++) {
        if (is_convex) {
            value += c[i] * std::pow(x[i], e[i]);
        } else {
            value += c[i] * x[i];
        }
    }
    return value;
}

double BaseModel::getObjectiveValue(const DoubleVec_t& x, const DoubleVec_t& x_prev) const
{
    return getObjectiveValue(x);
}

const DoubleVec_t& BaseModel::getObjectiveValueDerivative(const DoubleVec_t& x)
{
    for (uint32_t i = 0; i < nb_variables; i++) {
        dc[i] = (c[i] * e[i]) * std::pow(x[i], (e[i] - 1));
    }
    return dc;
}

const DoubleVec_t& BaseModel::getObjectiveValueDerivative(const DoubleVec_t& x, const DoubleVec_t& x_prev)
{
    return getObjectiveValueDerivative(x);
}
