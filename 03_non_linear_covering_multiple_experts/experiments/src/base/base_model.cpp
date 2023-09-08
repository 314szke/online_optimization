#include "base_model.h"

#include <sstream>


BaseModel::BaseModel() :
    nb_machines(0),
    nb_jobs(0),
    nb_variables(0),
    nb_constraints(0)
{}

uint32_t BaseModel::getId(uint32_t i, uint32_t j) const
{
    // Convert machine and job indices to one index
    return (j * nb_machines) + i;
}

uint32_t BaseModel::getNbMachines() const
{
    return nb_machines;
}

uint32_t BaseModel::getNbJobs() const
{
    return nb_jobs;
}

uint32_t BaseModel::getNbVariables() const
{
    return nb_variables;
}

uint32_t BaseModel::getNbConstraints() const
{
    return nb_constraints;
}

uint32_t BaseModel::getNbNewConstraints(const uint32_t time) const
{
    if (time >= b.size()) {
        return 0;
    }
    return b[time].size();
}

const DoubleVec_t& BaseModel::getCost() const
{
    return c;
}

const DoubleVec_t& BaseModel::getBound() const
{
    return b[0];
}

const DoubleVec_t& BaseModel::getBound(const uint32_t time) const
{
    if (time >= b.size()) {
        return empty_bound;
    }
    return b[time];
}

const DoubleMat_t& BaseModel::getCoefficient() const
{
    return A[0];
}

const DoubleMat_t& BaseModel::getCoefficient(const uint32_t time) const
{
    if (time >= A.size()) {
        return empty_coefficient;
    }
    return A[time];
}
