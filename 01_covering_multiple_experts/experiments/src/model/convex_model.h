#pragma once

#include "base/base_model.h"
#include "config/config.h"
#include "model/offline_model.h"
#include "model/experts.h"
#include "types/local_types.h"


class ConvexModel : public BaseModel {
public:
    ConvexModel(const Config& config, const OfflineModel& model, const Experts& experts);

    void revealNextConstraints();

    double getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    void calculateObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);

    uint32_t getNbLPVariables() const;
private:
    double getLinearObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    double getConvexObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    void calculateLinearObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);
    void calculateConvexObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);

    const OfflineModel& _offline_model;
    const Experts& _experts;

    uint32_t time;
    uint32_t nb_constraints_per_time;
    uint32_t nb_revealed_constraints;
    bool is_convex;
};
