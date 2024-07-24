#include "predictor.h"

#include <algorithm>


Predictor::Predictor(Graph& graph, std::vector<Scenario>& scenarios) :
    _graph(graph),
    _scenarios(scenarios)
{
    updateWeights(0);
}

void Predictor::updateWeights(uint32_t terminal)
{
    double sum_probabilities = 0.0;
    for (uint32_t idx = 0; idx < _scenarios.size(); idx++) {
        _scenarios[idx].checkIfStillFeasible(terminal);
        if (_scenarios[idx].feasible) {
            sum_probabilities += _scenarios[idx].probability;
        }
    }

    double weight;
    for (uint32_t e = 0; e < _graph.getNbEdges(); e++) {
        weight = 0.0;
        for (uint32_t idx = 0; idx < _scenarios.size(); idx++) {
            if (_scenarios[idx].feasible && _scenarios[idx].containsEdge(e)) {
                weight += _scenarios[idx].probability;
            }
        }
        weight = weight / sum_probabilities;
        _graph.setWeight(e, weight);
    }
}
