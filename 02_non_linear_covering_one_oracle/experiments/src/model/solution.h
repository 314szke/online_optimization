#pragma once

#include "model/model.h"
#include "types/local_types.h"


class Solution {
public:
    Solution(const Model& model, const DoubleVec_t& edges, uint32_t s, uint32_t t);

    UIntMat_t paths;
    UIntMat_t vertices;
    DoubleVec_t ratios;

private:
    UIntMat_t findAllPaths(uint32_t s, uint32_t t, const DoubleVec_t& edges);
    const Model& _model;
};
