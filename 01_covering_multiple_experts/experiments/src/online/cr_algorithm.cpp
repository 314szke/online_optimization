#include "cr_algorithm.h"

#include <sstream>

#include "model/solution.h"
#include "visualization/print.hpp"


CR_Algorithm::CR_Algorithm(const OfflineModel& model, const Config& config, const Experts& experts) :
    _model(model),
    _experts(experts),
    convex_model(config, model, experts),
    lp_solver(convex_model, config.gurobi_verbosity),
    frank_wolfe(config, convex_model, lp_solver),
    online_objective(0.0),
    sub_solutions(_model.getNbConstraints()),
    online_solution(model.getNbVariables(), 0.0),
    previous_solution(model.getNbVariables(), 0.0)
{
    for (uint32_t t = 0; t < _model.getNbConstraints(); t++) {
        sub_solutions[t].resize(model.getNbVariables(), 0.0);
    }
}

const DoubleVec_t& CR_Algorithm::solve()
{
    double temp_x = 0.0;

    for (uint32_t t = 1; t < (_model.getNbConstraints() + 1); t++) {
        convex_model.revealNextConstraints();
        lp_solver.addNewConstraints(t);

        DoubleVec_t w = frank_wolfe.solve();
        verifyWeights(w);
        roundWeightsIfNeeded(w);
        const DoubleMat_t& s = _experts.getSolutions((t - 1));

        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            temp_x = 0.0;
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                temp_x += s[k][i] * w[(i + (k * _model.getNbVariables()))];
            }
            if (previous_solution[i] < temp_x) {
                online_solution[i] = temp_x;
            }
        }

        for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
            sub_solutions[(t-1)][i] = online_solution[i];
        }
        Solution::RoundSolutionIfNeeded(_model, sub_solutions[(t-1)], t);

        verifySolution(t);
        online_objective = _model.getObjectiveValue(online_solution);
        previous_solution = online_solution;
    }

    return online_solution;
}

const DoubleMat_t& CR_Algorithm::getSubSolutions() const
{
    return sub_solutions;
}

double CR_Algorithm::getObjectiveValue() const
{
    return online_objective;
}

void CR_Algorithm::verifySolution(const uint32_t t) {
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
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
    for (uint32_t i = 0; i < _model.getNbVariables(); i++) {
        while (sumOfWeightsAreLessThanOne(w, i)) {
            for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
                w[(i + (k * _model.getNbVariables()))] += 0.0000000000001;
            }
        }
    }
}

bool CR_Algorithm::sumOfWeightsAreLessThanOne(const DoubleVec_t& w, uint32_t i)
{
    double value = 0.0;
    for (uint32_t k = 0; k < _experts.getNbExperts(); k++) {
        value += w[(i + (k * _model.getNbVariables()))];
    }

    if (value < 1.0 && ((1.0 - value) > 0.001)) {
        std::stringstream message;
        message << "ERROR: Weights are too small for variable " << i << " !" << std::endl;
        throw std::runtime_error(message.str());
    }
    return (value < 1.0);
}
