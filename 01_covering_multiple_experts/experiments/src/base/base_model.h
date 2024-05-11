#pragma once

#include "types/local_types.h"


class BaseModel {
public:
    bool isConvex() const;
    bool isOnline() const;
    bool useBoxConstraints() const;

    uint32_t getNbVariables() const;
    uint32_t getNbObjectiveVariables() const;
    uint32_t getNbConstraints() const;
    uint32_t getNbInitialConstraints() const;
    uint32_t getNbRevealedConstraints() const;
    uint32_t getNbConstraintBatches() const;
    uint32_t getConstraintBatchSize() const;
    uint32_t getNbConstraintsAt(const uint32_t current_time) const;
    uint32_t getNbSteps() const;

    const DoubleVec_t& getCost() const;
    const DoubleVec_t& getCostExponent() const;
    const DoubleVec_t& getBound(const uint32_t current_time) const;
    const DoubleMat_t& getCoefficient(const uint32_t current_time) const;

    double getObjectiveValue(const DoubleVec_t& x) const;
    virtual double getObjectiveValue(const DoubleVec_t& x, const DoubleVec_t& x_prev) const;
    const DoubleVec_t& getObjectiveValueDerivative(const DoubleVec_t& x);
    virtual const DoubleVec_t& getObjectiveValueDerivative(const DoubleVec_t& x, const DoubleVec_t& x_prev);
protected:
    BaseModel();

    bool is_convex;
    bool is_online;
    bool use_box_constraints;

    uint32_t nb_variables;
    uint32_t nb_constraints;
    uint32_t nb_objective_variables;
    uint32_t nb_initial_constraints;

    uint32_t nb_batches;
    uint32_t batch_size;
    uint32_t time;
    uint32_t nb_revealed_constraints;


    // Online Linear Program parameters
    DoubleVec_t c;
    DoubleVec_t dc;
    DoubleVec_t e;
    DoubleMat_t b;
    DoubleMatVec_t A;

    DoubleVec_t empty_bound;
    DoubleMat_t empty_coefficient;
};
