#include "std_algorithm.h"

#include "model/solution.h"


#define DT 0.0001


STD_Algorithm::STD_Algorithm(const OfflineModel& model) :
    _model(model),
    A(model.getCoefficient(0)),
    b(model.getBound(0)),
    solution(model.getNbVariables(), 0.0),
    sub_solutions(_model.getNbConstraints())
{
    for (uint32_t t = 0; t < _model.getNbConstraints(); t++) {
        sub_solutions[t].resize(model.getNbVariables(), 0.0);
    }
}

const DoubleVec_t& STD_Algorithm::solve() {
    const DoubleVec_t&c = _model.getCost();
    double constr_value, increasing_rate;

    for (uint32_t j = 0; j < _model.getNbConstraints(); j++) {
        constr_value = getConstraintValue(j);
        while (constr_value < b[j]) {
            for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
                if (A[j][i] != 0.0) {
                    increasing_rate = (A[j][i] / c[i]) * (solution[i] + (1.0 / _model.getNbVariables()));
                    solution[i] += increasing_rate * DT;
                }
            }
            constr_value = getConstraintValue(j);
        }

        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            sub_solutions[j][i] = solution[i];
        }
        Solution::RoundSolutionIfNeeded(_model, sub_solutions[j], (j+1));
    }
    return solution;
}

const DoubleMat_t& STD_Algorithm::getSubSolutions() const
{
    return sub_solutions;
}

double STD_Algorithm::getObjectiveValue() const
{
    return _model.getObjectiveValue(solution);
}

double STD_Algorithm::getConstraintValue(const uint32_t j)
{
    double constr_value = 0.0;
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        constr_value += A[j][i] * solution[i];
    }
    return constr_value;
}
