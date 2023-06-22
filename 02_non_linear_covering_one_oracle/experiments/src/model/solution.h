#pragma once

#include "model/model.h"
#include "types/local_types.h"


class Solution {
public:
    Solution(const Model& model, const DoubleVec_t& edges, uint32_t s, uint32_t t);

    UIntMat_t paths;
    DoubleVec_t ratios;

private:
    UIntMat_t findPathFrom(uint32_t s, const BoolVec_t& visited);
    const Model& _model;
};
