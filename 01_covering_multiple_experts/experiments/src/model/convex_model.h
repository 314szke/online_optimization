#pragma once

#include "base/base_model.h"
#include "model/offline_model.h"
#include "model/experts.h"
#include "types/local_types.h"


class ConvexModel : public BaseModel {
public:
    ConvexModel(const OfflineModel& model, const Experts& experts);

    void revealNextConstraint();

    double getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    void calculateObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);

    uint32_t getNbLPVariables() const;
private:
    const OfflineModel& _offline_model;
    const Experts& _experts;

    uint32_t time;
    uint32_t nb_constraints_per_time;
    uint32_t nb_revealed_constraints;
};
