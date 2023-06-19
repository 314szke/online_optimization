#pragma once

#include<vector>

#include "model/exprtk.hpp"


typedef std::vector<bool> BoolVec_t;
typedef std::vector<double> DoubleVec_t;
typedef std::vector<int64_t> IntVec_t;
typedef std::vector<uint32_t> UIntVec_t;

typedef std::vector<BoolVec_t> BoolMat_t;
typedef std::vector<DoubleVec_t> DoubleMat_t;
typedef std::vector<IntVec_t> IntMat_t;
typedef std::vector<UIntVec_t> UIntMat_t;
typedef std::vector<UIntMat_t> UIntMatVec_t;


class IndexValuePair {
public:
    IndexValuePair(const uint32_t i, const double v) :
        idx(i),
        value(v)
    {}

    friend bool operator <(const IndexValuePair& x, const IndexValuePair& y) {
        return std::tie(x.value, x.idx) < std::tie(y.value, y.idx);
    }

    uint32_t idx;
    double value;
};

typedef std::vector<IndexValuePair> IndexValuePairVec_t;
