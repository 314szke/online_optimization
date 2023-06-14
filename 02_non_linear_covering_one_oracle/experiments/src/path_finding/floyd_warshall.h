#pragma once

#include "model/model.h"
#include "types/local_types.h"


class FloydWarshall {
public:
    FloydWarshall(const Model& model);
    const DoubleVec_t& getReachableEdges(const uint32_t request_id) const;
    uint32_t getMaxDimension() const;
private:
    UIntMat_t D;
    DoubleMat_t allowed_edges;
    uint32_t max_dimension;
};
