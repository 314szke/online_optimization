#pragma once

#include<vector>

#include "model/exprtk.hpp"


typedef std::vector<int64_t> IntVec_t;
typedef std::vector<uint32_t> UIntVec_t;
typedef std::vector<double> DoubleVec_t;
typedef std::vector<UIntVec_t> UIntMat_t;
typedef std::vector<UIntMat_t> UIntMatVec_t;
typedef std::vector<DoubleVec_t> DoubleMat_t;
typedef std::vector<bool> BoolVec_t;
typedef std::vector<BoolVec_t> BoolMat_t;

typedef exprtk::symbol_table<double> SymbolTable_t;
typedef exprtk::expression<double> Expression_t;
typedef std::vector<Expression_t> ExpressionVec_t;
typedef exprtk::parser<double> FormulaParser_t;


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
