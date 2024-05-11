#pragma once

#include "model/offline_model.h"
#include "types/local_types.h"


class DummyExpert
{
public:
    DummyExpert(const OfflineModel& off_model);
    const DoubleVec_t& getSolution(const uint32_t time) const;
    const DoubleVec_t& getFinalSolution() const;
private:
    double init_value;
    DoubleMat_t dummy_solution;
};
