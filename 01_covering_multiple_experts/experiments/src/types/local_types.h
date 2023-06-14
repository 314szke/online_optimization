#pragma once

#include <cstdint>
#include <vector>


typedef std::vector<bool> BoolVec_t;
typedef std::vector<double> DoubleVec_t;
typedef std::vector<int64_t> IntVec_t;
typedef std::vector<uint32_t> UIntVec_t;

typedef std::vector<BoolVec_t> BoolMat_t;
typedef std::vector<DoubleVec_t> DoubleMat_t;
typedef std::vector<IntVec_t> IntMat_t;
typedef std::vector<UIntVec_t> UIntMat_t;

typedef std::vector<DoubleMat_t> DoubleMatVec_t;