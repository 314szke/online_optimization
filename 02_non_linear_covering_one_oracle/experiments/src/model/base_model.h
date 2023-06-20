#pragma once

#include "model.h"
#include "types/local_types.h"


class BaseModel {
public:
    uint32_t getNbVariables() const;
    uint32_t getNbConstraints() const;

    double getObjectiveValue();

    virtual void setCurrentSolution(const DoubleVec_t& x) = 0;

    DoubleVec_t& getCostVector();
    DoubleVec_t& getBounds();
    DoubleMat_t& getCoefficients();
protected:
    BaseModel(Model& model, uint32_t n, uint32_t m);

    Model& _model;

    uint32_t nb_variables;
    uint32_t nb_constraints;

    DoubleVec_t solution;
    DoubleVec_t current_solution;
    DoubleVec_t previous_solution;

    DoubleVec_t costs;
    DoubleVec_t bounds;
    DoubleMat_t coefficients;
};
