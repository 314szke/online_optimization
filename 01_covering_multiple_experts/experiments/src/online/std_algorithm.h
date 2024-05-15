#pragma once

#include "model/offline_model.h"
#include "model/online_model.h"


class STD_Algorithm {
public:
    STD_Algorithm(const OfflineModel& off_model);
    const DoubleVec_t& solve();
    const DoubleMat_t& getSubSolutions() const;
    double getObjectiveValue() const;
private:
    void setDerivatives(int time);
    const OfflineModel& offline_model;
    OnlineModel online_model;
    DoubleVec_t solution;
    DoubleMat_t sub_solutions;
    DoubleVec_t uniform_solution;
    DoubleVec_t derivative;
};
