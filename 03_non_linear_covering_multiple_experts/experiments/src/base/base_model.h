#pragma once

#include "types/local_types.h"


class BaseModel {
public:
    uint32_t getId(uint32_t i, uint32_t j) const;

    uint32_t getNbMachines() const;
    uint32_t getNbJobs() const;
    uint32_t getNbVariables() const;
    uint32_t getNbConstraints() const;
    uint32_t getNbNewConstraints(const uint32_t time) const;

    const DoubleVec_t& getCost() const;
    const DoubleVec_t& getBound() const;
    const DoubleMat_t& getCoefficient() const;
    const DoubleVec_t& getBound(const uint32_t time) const;
    const DoubleMat_t& getCoefficient(const uint32_t time) const;
protected:
    BaseModel();

    uint32_t nb_machines;
    uint32_t nb_jobs;
    uint32_t nb_variables;
    uint32_t nb_constraints;

    // Online Linear Program parameters
    DoubleVec_t c;
    DoubleMat_t b;
    DoubleMatVec_t A;

    DoubleVec_t empty_bound;
    DoubleMat_t empty_coefficient;
};
