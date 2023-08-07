#include "offline_solver.h"

#include <limits>
#include <sstream>


OfflineSolver::OfflineSolver(const BaseModel& model, uint32_t verbosity) :
    base_model(model),
    obj_value(0.0),
    env(true)
{
    env.set("LogFile", "gurobi.log");
    env.set(GRB_IntParam_OutputFlag, verbosity);
    env.start();
    lp_model.reset(new GRBModel(env));

    x.resize(base_model.getNbVariables(), 0.0);

    // Add variables
    for (uint32_t i = 0; i < base_model.getNbVariables(); i++) {
        std::stringstream ss;
        ss << "x_" << i;
        GRBVar var = lp_model->addVar(0, 1, 0, GRB_CONTINUOUS, ss.str());
        variables.push_back(var);
    }

    // Add auxilary variables
    y = lp_model->addVar(0, std::numeric_limits<double>::infinity(), 0, GRB_CONTINUOUS, "y");

    // Add constraints
    const DoubleVec_t& b = base_model.getBound();
    const DoubleMat_t& A = base_model.getCoefficient();

    for (uint32_t j = 0; j < base_model.getNbConstraints(); j++) {
        GRBLinExpr expr;
        for (uint32_t i = 0; i < base_model.getNbVariables(); i++) {
            if (A[j][i] != 0) {
                expr += A[j][i] * variables[i];
            }
        }
        std::stringstream ss;
        ss << "c_" << j;
        GRBConstr constraint = lp_model->addConstr(expr >= b[j], ss.str());
        constraints.push_back(constraint);
    }

    // Add auxilary constraints
    const DoubleVec_t& c = base_model.getCost();
    uint32_t idx;

    for (uint32_t i = 0; i < base_model.getNbMachines(); i++) {
        GRBLinExpr expr;
        for (uint32_t j = 0; j < base_model.getNbJobs(); j++) {
            idx = base_model.getId(i, j);
            expr += c[idx] * variables[idx];
        }
        expr += -1 * y;

        std::stringstream ss;
        ss << "l_" << i;
        GRBConstr constraint = lp_model->addConstr(expr <= 0, ss.str());
        constraints.push_back(constraint);
    }
}

const DoubleVec_t& OfflineSolver::solve()
{
    GRBLinExpr objective;
    objective += y;
    lp_model->setObjective(objective, GRB_MINIMIZE);

    // Debug code
    //try {
    //    lp_model->computeIIS();
    //    lp_model->write("model.ilp");
    //} catch(...) {}
    lp_model->write("model.lp");

    lp_model->optimize();

    obj_value = lp_model->get(GRB_DoubleAttr_ObjVal);
    for (uint32_t i = 0; i < base_model.getNbVariables(); i++) {
        x[i] = variables[i].get(GRB_DoubleAttr_X);
    }

    return x;
}

double OfflineSolver::getObjectiveValue() const
{
    return obj_value;
}
