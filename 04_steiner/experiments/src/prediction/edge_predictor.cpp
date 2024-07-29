#include "edge_predictor.h"

#include <algorithm>
#include <iostream>


EdgePredictor::EdgePredictor(Graph& graph, std::vector<Scenario>& scenarios, double lambda) :
    _graph(graph),
    _scenarios(scenarios),
    _lambda(lambda)
{
    updateWeights(0);
}

void EdgePredictor::updateWeights(uint32_t terminal)
{
    double sum_probabilities = 0.0;
    for (uint32_t idx = 0; idx < _scenarios.size(); idx++) {
        _scenarios[idx].checkIfStillFeasible(terminal);
        if (_scenarios[idx].feasible) {
            sum_probabilities += _scenarios[idx].probability;
        }
    }

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
        if (weight > 0.5) {
            _graph.setWeight(e, weight);
        } else {
            _graph.setWeight(e, 0.0);
        }
    }
}

void EdgePredictor::discountWeights()
{
    std::cout << "EDGES are discounted!" << std::endl;
    double weight;
    for (uint32_t edge = 0; edge < _graph.getNbEdges(); edge++) {
        weight = _graph.getWeight(edge);
        weight = weight * _lambda;
        _graph.setWeight(edge, weight);
    }
}

bool EdgePredictor::supportsSubSolution(const std::vector<uint32_t>& sub_solution)
{
    for (uint32_t idx = 0; idx < _scenarios.size(); idx++) {
        if (_scenarios[idx].feasible && _scenarios[idx].containsSolution(_graph, sub_solution)) {
            return true;
        }
    }
    return false;
}
