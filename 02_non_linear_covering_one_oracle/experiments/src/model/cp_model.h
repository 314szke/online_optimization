#pragma once

#include "model.h"


class CP_Model {
public:
    CP_Model(Model& model);

    double getObjectiveValue();
    uint32_t getNbVariables() const;

    const DoubleVec_t& getCostVector();
    const DoubleVec_t& getBounds();
    const DoubleMat_t& getCoefficients();
private:
    uint32_t getID(uint32_t e, uint32_t r) const;

    Model& _model;

    uint32_t nb_variables;
    uint32_t nb_constraints;

    DoubleVec_t costs;
    DoubleVec_t bounds;
    DoubleMat_t coefficients;
};
