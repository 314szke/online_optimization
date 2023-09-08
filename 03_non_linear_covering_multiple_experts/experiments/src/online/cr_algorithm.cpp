#include "cr_algorithm.h"

#include <sstream>

#include "model/solution.h"
#include "visualization/print.hpp"


CR_Algorithm::CR_Algorithm(const OfflineModel& model, const Config& config, const Experts& experts) :
    offline_model(model),
    _experts(experts),
    convex_model(model, experts),
    frank_wolfe(config, convex_model),
    online_objective(0.0),
    online_solution(offline_model.getNbVariables(), 0.0),
    previous_solution(offline_model.getNbVariables(), 0.0)
{}

const DoubleVec_t& CR_Algorithm::solve()
{
    double temp_x = 0.0;
    uint32_t idx;

    for (uint32_t t = 0; t < offline_model.getNbConstraints(); t++) {
        convex_model.revealNextConstraint();

        DoubleVec_t w = frank_wolfe.solve();
        verifyWeights(w);
        roundWeightsIfNeeded(w);
        const DoubleMat_t& s = _experts.getSolutions(t);

        for (uint32_t i = 0; i < convex_model.getNbMachines(); i++) {
            temp_x = 0.0;
            idx = (t * convex_model.getNbMachines()) + i;
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                temp_x += s[k][idx] * w[((k * convex_model.getNbMachines()) + i)];
            }
            if (previous_solution[idx] < temp_x) {
                online_solution[idx] = temp_x;
            }
        }

        verifySolution(t);
        online_objective = offline_model.getObjectiveValue(online_solution);
        previous_solution = online_solution;
        convex_model.updateSolution(online_solution);
    }

    return online_solution;
}

double CR_Algorithm::getObjectiveValue() const
{
    return online_objective;
}

void CR_Algorithm::verifySolution(const uint32_t t) {
    for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
        if (online_solution[i] < previous_solution[i]) {
            std::stringstream message;
            message << "ERROR: solution at constraint " << t << " is invalid!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }
}

void CR_Algorithm::verifyWeights(const DoubleVec_t& w)
{
    for (uint32_t v = 0; v < convex_model.getNbVariables(); v++) {
        if (w[v] < 0) {
            std::stringstream message;
            message << "ERROR: Weight " << v << " is negative!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }
}

void CR_Algorithm::roundWeightsIfNeeded(DoubleVec_t& w)
{
    for (uint32_t i = 0; i < convex_model.getNbMachines(); i++) {
        while (sumOfWeightsAreLessThanOne(w, i)) {
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                w[(i + (k * convex_model.getNbMachines()))] += 0.0000000000001;
            }
        }
    }
}

bool CR_Algorithm::sumOfWeightsAreLessThanOne(const DoubleVec_t& w, uint32_t i)
{
    double value = 0.0;
    for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
        value += w[(i + (k * convex_model.getNbMachines()))];
    }

    if (value < 1.0 && ((1.0 - value) > 0.001)) {
        std::stringstream message;
        message << "ERROR: Weights are too small for variable " << i << " !" << std::endl;
        throw std::runtime_error(message.str());
    }
    return (value < 1.0);
}
