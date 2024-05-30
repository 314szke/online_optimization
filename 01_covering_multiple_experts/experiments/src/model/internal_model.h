#pragma once

#include "base/base_model.h"
#include "config/config.h"
#include "model/offline_model.h"
#include "model/online_model.h"
#include "model/experts.h"
#include "types/local_types.h"


class InternalModel : public BaseModel {
public:
    InternalModel(const Config& config, const OfflineModel& model, const Experts& experts);

    void revealNextConstraints();

    double getObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    const DoubleVec_t& getObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);

    const DoubleVec_t& getInitialSolution() const;
    uint32_t getNbOriginalRevealedConstraints() const;
private:
    double getLinearObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    double getConvexObjectiveValue(const DoubleVec_t& w, const DoubleVec_t& w_prev) const;
    void calculateLinearObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);
    void calculateConvexObjectiveValueDerivative(const DoubleVec_t& w, const DoubleVec_t& w_prev);

    DoubleVec_t initial_solution;
    OnlineModel online_model;
    const Experts& _experts;
    double L;
};
