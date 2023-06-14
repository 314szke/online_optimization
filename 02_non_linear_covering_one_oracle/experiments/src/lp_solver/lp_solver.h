#pragma once

#include <vector>

#include "gurobi_c++.h"

#include "model/model.h"
#include "types/local_types.h"


class LP_Solver {
public:
    typedef std::unique_ptr<GRBModel> Model_t;
    typedef std::vector<GRBVar> VarVec_t;
    typedef std::vector<VarVec_t> VarMat_t;
    typedef std::vector<GRBConstr> ConstrVec_t;

    LP_Solver(Model& model, const Model::RequestVec_t& requests);
    double solve(const DoubleMat_t& x, DoubleMat_t& v, const DoubleVec_t& extra_cost);
private:
    Model& _model;
    const Model::RequestVec_t& _requests;

    GRBEnv env;
    Model_t lp_model;

    VarMat_t variables;
    ConstrVec_t constraints;
};
