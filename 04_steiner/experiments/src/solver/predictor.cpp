#include "predictor.h"

#include <algorithm>


Predictor::Predictor(Graph& graph, std::vector<Scenario>& scenarios) :
    _graph(graph),
    _scenarios(scenarios)
{
    std::vector<uint32_t> no_terminals;
    updateWeights(no_terminals);
}

void Predictor::updateWeights(const std::vector<uint32_t>& terminals)
{
    std::vector<uint32_t> ordered_terminals = terminals;
    std::sort(ordered_terminals.begin(), ordered_terminals.end());

    double sum_probabilities = 0.0;
    for (auto scenario : _scenarios) {
        scenario.checkIfStillFeasible(terminals);
        if (scenario.feasible) {
            sum_probabilities += scenario.probability;
        }
    }

    double weight;
    for (uint32_t e = 0; e < _graph.getNbEdges(); e++) {
        weight = 0.0;
        for (auto scenario : _scenarios) {
            if (scenario.feasible && scenario.containsEdge(e)) {
                weight += scenario.probability;
            }
        }
        weight = weight / sum_probabilities;
        _graph.setWeight(e, weight);
    }
}
