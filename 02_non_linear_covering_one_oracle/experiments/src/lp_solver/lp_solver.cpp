#include "lp_solver.h"

#include<sstream>


LP_Solver::LP_Solver(Model& model, const Model::RequestVec_t& requests) :
    _model(model),
    _requests(requests),
    env(true)
{
    env.set("LogFile", "gurobi.log");
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();
    lp_model.reset(new GRBModel(env));

    // Add variables
    for (uint32_t r = 0; r < _requests.size(); r++) {
        variables.push_back(VarVec_t());
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            std::stringstream ss;
            ss << "x_" << r << "_" << e;
            GRBVar var = lp_model->addVar(0, 1, 0, GRB_CONTINUOUS, ss.str());
            variables[r].push_back(var);
        }
    }

    // Add flow constraints
    const Model::Graph_t& A =_model.getAdjacencyMatrix();

    for (uint32_t r = 0; r < _requests.size(); r++) {
        for (uint32_t i = 0; i < _model.getNbVertices(); i++) {
            if (i != _requests[r].source && i != _requests[r].target) {
                GRBLinExpr flow_const;
                uint32_t counter = 0;
                for (uint32_t j = 0; j < _model.getNbVertices(); j++) {
                    if (A[i][j] != -1) {
                        flow_const += 1 * variables[r][A[i][j]];
                        counter++;
                    }
                    if (A[j][i] != -1) {
                        flow_const += -1 * variables[r][A[j][i]];
                        counter++;
                    }
                }
                if (counter > 0) {
                    std::stringstream ss;
                    ss << "c_" << r << "_" << i;
                    GRBConstr constr = lp_model->addConstr(flow_const >= 0, ss.str());
                    constraints.push_back(constr);
                }
            }
        }
    }

    for (uint32_t idx = 0; idx < _requests.size(); idx++) {
        GRBLinExpr flow_const;
        uint32_t i = _requests[idx].source;
        for (uint32_t j = 0; j < _model.getNbVertices(); j++) {
            if (A[i][j] != -1) {
                flow_const += 1 * variables[idx][A[i][j]];
            }
            if (A[j][i] != -1) {
                flow_const += -1 * variables[idx][A[j][i]];
            }
        }
        std::stringstream ss1;
        ss1 << "cf_" << idx << "_" << i;
        GRBConstr constr1 = lp_model->addConstr(flow_const >= 1, ss1.str());
        constraints.push_back(constr1);

        uint32_t j = _requests[idx].target;
        for (uint32_t i = 0; i < _model.getNbVertices(); i++) {
            if (A[i][j] != -1) {
                flow_const += 1 * variables[idx][A[i][j]];
            }
            if (A[j][i] != -1) {
                flow_const += -1 * variables[idx][A[j][i]];
            }
        }
        std::stringstream ss2;
        ss2 << "cf_" << idx << "_" << j;
        GRBConstr constr2 = lp_model->addConstr(flow_const >= 1, ss2.str());
        constraints.push_back(constr2);
    }
}

double LP_Solver::solve(const DoubleMat_t& x, DoubleMat_t& v, const DoubleVec_t& extra_cost)
{
    double sum = 0.0;
    double derivative = 0.0;
    uint32_t nb_non_zero = 0;

    GRBLinExpr objective;
    for (uint32_t idx = 0; idx < _model.getNbEdges(); idx++) {
        sum = 0.0;
        nb_non_zero = 0;
        for (uint32_t r = 0; r < _requests.size(); r++) {
            sum += x[r][idx];
            if (x[r][idx] != 0) {
                nb_non_zero++;
            }
        }
        sum += extra_cost[idx];
        derivative = _model.getEdgeDerivative(idx, sum);

        for (uint32_t r = 0; r < _requests.size(); r++) {
            if (x[r][idx] != 0) {
                objective += (derivative / nb_non_zero) * variables[r][idx];
            }
        }
    }

    lp_model->setObjective(objective, GRB_MINIMIZE);
    //lp_model->write("model.lp");
    lp_model->optimize();

    for (uint32_t r = 0; r < _requests.size(); r++) {
        for (uint32_t idx = 0; idx < _model.getNbEdges(); idx++) {
            v[r][idx] = variables[r][idx].get(GRB_DoubleAttr_X);
        }
    }

    return lp_model->get(GRB_DoubleAttr_ObjVal);
}
