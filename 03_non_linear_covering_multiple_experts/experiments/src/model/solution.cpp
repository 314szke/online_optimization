#include "solution.h"

#include <sstream>


void Solution::RoundSolutionIfNeeded(const OfflineModel& model, DoubleVec_t& x, uint32_t constr_limit)
{
    while (ConstraintIsNotSatisfied(model, x, constr_limit)) {
        for (uint32_t i = 0; i < model.getNbVariables(); i++) {
            x[i] += 0.0000000000001;
        }
    }
}

bool Solution::ConstraintIsNotSatisfied(const OfflineModel& model, const DoubleVec_t& x, uint32_t constr_limit)
{
    const DoubleVec_t& b = model.getBound(0);
    const DoubleMat_t& A = model.getCoefficient(0);

    for (uint32_t j = 0; j < constr_limit; j++) {
        double value = 0.0;
        for (uint32_t i = 0; i < model.getNbVariables(); i++) {
            value += A[j][i] * x[i];
        }
        if (value < b[j]) {
            if ((b[j] - value) > 0.001) {
                std::stringstream message;
                message << "ERROR: Solution " << value << " is too small for constraint " << j << " !" << std::endl;
                throw std::runtime_error(message.str());
            }
            return true;
        }
    }
    return false;
}
