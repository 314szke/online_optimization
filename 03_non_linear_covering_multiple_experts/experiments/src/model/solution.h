#pragma once

#include "model/offline_model.h"
#include "types/local_types.h"


class Solution {
public:
    static void RoundSolutionIfNeeded(const OfflineModel& model, DoubleVec_t& x, uint32_t constr_limit);
    static bool ConstraintIsNotSatisfied(const OfflineModel& model, const DoubleVec_t& x, uint32_t constr_limit);
};
