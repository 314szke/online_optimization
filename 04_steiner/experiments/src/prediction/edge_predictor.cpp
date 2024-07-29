#include "edge_predictor.h"

#include <algorithm>
#include <iostream>
#include <limits>


EdgePredictor::EdgePredictor(Graph& graph, std::vector<Scenario>& scenarios, double limit) :
    _graph(graph),
    edge_weight_limit(limit),
    _scenarios(scenarios)
{
    updateWeights(0);
}

void EdgePredictor::updateWeights(uint32_t terminal)
{
    double min_probability = std::numeric_limits<double>::max();std::numeric_limits<double>::max();
    double max_probability = 0.0;
    double sum_probabilities = 0.0;
    uint32_t counter = 0;
    for (uint32_t idx = 0; idx < _scenarios.size(); idx++) {
        _scenarios[idx].checkIfStillFeasible(terminal);
        if (_scenarios[idx].feasible) {
            counter++;
            sum_probabilities += _scenarios[idx].probability;
            if (_scenarios[idx].probability < min_probability) {
                min_probability = _scenarios[idx].probability;
            }
            if (_scenarios[idx].probability > max_probability) {
                max_probability = _scenarios[idx].probability;
            }
        }
    }
    if (counter <= 1) {
        edge_weight_limit = 0.0;
    } else {
        edge_weight_limit = min_probability / max_probability;
    }
    std::cout << "LAMBDA = " << edge_weight_limit << std::endl;

    // No feasible scenarios left, we know nothing about the future
    if (sum_probabilities == 0.0) {
        for (uint32_t e = 0; e < _graph.getNbEdges(); e++) {
            _graph.setWeight(e, 0.0);
        }
        return;
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
        if (weight > edge_weight_limit) {
            _graph.setWeight(e, weight);
        } else {
            _graph.setWeight(e, 0.0);
        }
    }
}
