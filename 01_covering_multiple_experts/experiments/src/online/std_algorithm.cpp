#include "std_algorithm.h"

#include "model/solution.h"


#define DT 0.0001


STD_Algorithm::STD_Algorithm(const OfflineModel& off_model) :
    offline_model(off_model),
    online_model(off_model),
    solution(offline_model.getNbVariables(), 0.0),
    sub_solutions(offline_model.getNbConstraintBatches()),
    uniform_solution(offline_model.getNbVariables(), 0.0001),
    derivative(offline_model.getNbVariables(), 0.0)
{
    for (uint32_t t = 0; t < offline_model.getNbConstraintBatches(); t++) {
        sub_solutions[t].resize(offline_model.getNbVariables(), 0.0);
    }
}

const DoubleVec_t& STD_Algorithm::solve() {
    const DoubleVec_t& c = online_model.getCost();
    setDerivatives(-1); // init step

    double increasing_rate;
    for (uint32_t t = 0; t < online_model.getNbConstraintBatches(); t++) {
        online_model.revealNextConstraints();
        const DoubleMat_t& A = online_model.getCoefficient(t + 1);

        while (!online_model.isSatisfiedBy(solution)) {
            for (uint32_t j = 0; j < online_model.getConstraintBatchSize(); j++) {
                for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
                    if (A[j][i] != 0.0) {
                        if (online_model.isConvex()) {
                            increasing_rate = (1.0 / online_model.getConstraintBatchSize()) * (A[j][i] / derivative[i]) * (solution[i] + (1.0 / online_model.getNbVariables()));
                        } else {
                            increasing_rate = (1.0 / online_model.getConstraintBatchSize()) * (A[j][i] / c[i]) * (solution[i] + (1.0 / online_model.getNbVariables()));
                        }
                        solution[i] += increasing_rate * DT;
                    }
                }
            }
        }

        for (uint32_t i = 0; i < online_model.getNbVariables(); i++) {
            sub_solutions[t][i] = solution[i];
        }
        Solution::RoundSolutionIfNeeded(offline_model, sub_solutions[t], online_model.getNbRevealedConstraints());
        setDerivatives(t);
    }
    return solution;
}

const DoubleMat_t& STD_Algorithm::getSubSolutions() const
{
    return sub_solutions;
}

double STD_Algorithm::getObjectiveValue() const
{
    return online_model.getObjectiveValue(solution);
}

void STD_Algorithm::setDerivatives(int time)
{
    if (time == -1) {
        const DoubleVec_t& dc_init = online_model.getObjectiveValueDerivative(uniform_solution);
        for (uint32_t i = 0; i < online_model.getNbObjectiveVariables(); i++) {
            derivative[i] = dc_init[i];
        }
    } else {
        const DoubleVec_t& dc = online_model.getObjectiveValueDerivative(solution);
        for (uint32_t i = 0; i < online_model.getNbObjectiveVariables(); i++) {
            derivative[i] = dc[i];
        }
    }
}
