#pragma once

#include "model/model.h"
#include "types/local_types.h"


// Conditional Breadth-First Search
class CBFS {
public:
    CBFS(const Model& model);
    UIntVec_t getPath(const uint32_t s, const uint32_t t, const BoolVec_t& marker);
private:
    bool pathExists(const uint32_t s, const uint32_t t, const BoolVec_t& marker);

    const Model& _model;
    BoolVec_t visited;
    IntVec_t prev;
};
