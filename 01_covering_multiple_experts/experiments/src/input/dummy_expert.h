#pragma once

#include "model/offline_model.h"
#include "types/local_types.h"


class DummyExpert
{
public:
    DummyExpert(const double epsilon, const OfflineModel& off_model);
    const DoubleVec_t& getSolution(const uint32_t time) const;
private:
    double init_value;
    DoubleMat_t dummy_solution;
};
