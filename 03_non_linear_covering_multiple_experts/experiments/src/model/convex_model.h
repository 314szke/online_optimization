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
    uint32_t getLocalId(uint32_t i, uint32_t k) const;
    double f(const DoubleVec_t& x) const;
    double d_i_f(const DoubleVec_t& x, uint32_t machine_i, uint32_t var_i) const;

    const OfflineModel& offline_model;
    const Experts& _experts;

    double p;
    double L;
    int64_t time;
    uint32_t nb_constraints_per_time;
};
