#include "base_model.h"

#include <sstream>


BaseModel::BaseModel() :
    is_minimization(true),
    is_convex(false),
    use_box_constraints(false),
    nb_variables(0),
    nb_constraints(0),
    batch_size(1),
    online_step(1)
{}

bool BaseModel::isMinimization() const
{
    return is_minimization;
}

bool BaseModel::useBoxConstraints() const
{
    return use_box_constraints;
}

uint32_t BaseModel::getNbVariables() const
{
    return nb_variables;
}

uint32_t BaseModel::getNbConstraints() const
{
    return (nb_constraints / batch_size);
}

uint32_t BaseModel::getNbNewConstraints(const uint32_t time) const
{
    if (time >= b.size()) {
        return 0;
    }
    return b[time].size();
}

uint32_t BaseModel::getNbOnlineSteps() const
{
    return online_step;
}

const DoubleVec_t& BaseModel::getCost() const
{
    return c;
}

const UIntVec_t& BaseModel::getCostExponent() const
{
    return e;
}

const DoubleVec_t& BaseModel::getBound(const uint32_t time) const
{
    if (time >= b.size()) {
        return empty_bound;
    }
    return b[time];
}

const DoubleMat_t& BaseModel::getCoefficient(const uint32_t time) const
{
    if (time >= A.size()) {
        return empty_coefficient;
    }
    return A[time];
}
