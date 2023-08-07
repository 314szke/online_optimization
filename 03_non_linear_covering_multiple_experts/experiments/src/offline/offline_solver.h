#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "gurobi_c++.h"

#include "base/base_model.h"
#include "types/local_types.h"


class OfflineSolver {
public:
    typedef std::unique_ptr<GRBModel> Model_t;
    typedef std::vector<GRBVar> VarVec_t;
    typedef std::vector<GRBConstr> ConstrVec_t;

    OfflineSolver(const BaseModel& model, uint32_t verbosity);
    const DoubleVec_t& solve();
    double getObjectiveValue() const;
private:
    const BaseModel& base_model;

    double obj_value;
    DoubleVec_t x;

    GRBEnv env;
    Model_t lp_model;

    GRBVar y;
    VarVec_t variables;
    ConstrVec_t constraints;
};
