#pragma once

#include "model/offline_model.h"


class STD_Algorithm {
public:
    STD_Algorithm(const OfflineModel& model);

    const DoubleVec_t& solve();
    const DoubleMat_t& getSubSolutions() const;
    double getObjectiveValue() const;
private:
    double getConstraintValue(const uint32_t j);

    const OfflineModel& _model;
    const DoubleMat_t& A;
    const DoubleVec_t& b;

    DoubleVec_t solution;
    DoubleMat_t sub_solutions;
};
