#include "lp_solver.h"

#include <limits>
#include <sstream>


LP_Solver::LP_Solver(const BaseModel& model, uint32_t verbosity) :
    _model(model),
    obj_value(0.0),
    env(true)
{
    env.set("LogFile", "gurobi.log");
    env.set(GRB_IntParam_OutputFlag, verbosity);
    env.start();
    lp_model.reset(new GRBModel(env));

    x.resize(_model.getNbVariables(), 0.0);

    // Add variables
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        std::stringstream ss;
        ss << "x_" << i;
        if (_model.useBoxConstraints()) {
            GRBVar var = lp_model->addVar(0.0, 1.0001, 0, GRB_CONTINUOUS, ss.str());
            variables.push_back(var);
        } else {
            GRBVar var = lp_model->addVar(0, std::numeric_limits<double>::infinity(), 0, GRB_CONTINUOUS, ss.str());
            variables.push_back(var);
        }
    }

    // Add constraints
    addNewConstraints(0);
}

void LP_Solver::addNewConstraints(const uint32_t time)
{
    const DoubleVec_t& b = _model.getBound(time);
    const DoubleMat_t& A = _model.getCoefficient(time);

    for (uint32_t j = 0; j < _model.getNbConstraintsAt(time); j++) {
        GRBLinExpr expr;
        uint32_t counter = 0;
        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            if (A[j][i] != 0) {
                expr += A[j][i] * variables[i];
                counter++;
            }
        }
        if (counter > 0 ) {
            std::stringstream ss;
            ss << "c_" << time << "_" << j;
            GRBConstr constraint = lp_model->addConstr(expr >= b[j], ss.str());
            constraints.push_back(constraint);
        }
    }
}

const DoubleVec_t& LP_Solver::solve()
{
    const DoubleVec_t& c = _model.getCost();
    GRBLinExpr objective;
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        objective += c[i] * variables[i];
    }

    lp_model->setObjective(objective, GRB_MINIMIZE);

    // Debug code
    //try {
    //    lp_model->computeIIS();
    //    lp_model->write("model.ilp");
    //} catch(...) {}
    //lp_model->write("model.lp");

    lp_model->optimize();

    obj_value = lp_model->get(GRB_DoubleAttr_ObjVal);
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        x[i] = variables[i].get(GRB_DoubleAttr_X);
    }

    return x;
}

double LP_Solver::getObjectiveValue() const
{
    return obj_value;
}
