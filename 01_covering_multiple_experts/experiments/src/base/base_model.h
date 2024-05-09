#pragma once

#include "types/local_types.h"


class BaseModel {
public:
    bool isMinimization() const;
    bool useBoxConstraints() const;

    uint32_t getNbVariables() const;
    uint32_t getNbConstraints() const;
    uint32_t getNbNewConstraints(const uint32_t time) const;
    uint32_t getNbOnlineSteps() const;

    const DoubleVec_t& getCost() const;
    const UIntVec_t& getCostExponent() const;
    const DoubleVec_t& getBound(const uint32_t time) const;
    const DoubleMat_t& getCoefficient(const uint32_t time) const;

    bool is_minimization;
    bool is_convex;
    bool use_box_constraints;
    uint32_t nb_variables;
    uint32_t nb_constraints;
    uint32_t batch_size;

    // Online Linear Program parameters
    DoubleVec_t c;
    UIntVec_t e;
    DoubleMat_t b;
    DoubleMatVec_t A;
    uint32_t online_step;

    DoubleVec_t empty_bound;
    DoubleMat_t empty_coefficient;

protected:
    BaseModel();
};
