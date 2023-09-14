#pragma once

#include "types/local_types.h"


class Oracle {
public:
    Oracle() :
        objective_value(0.0)
    {}

    UIntVec_t dimensions;
    DoubleMat_t predictions;
    double objective_value;
};
