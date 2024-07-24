#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/scenario.h"


class EdgePredictor {
public:
    EdgePredictor(Graph& graph, std::vector<Scenario>& scenarios, double lambda);
    void updateWeights(uint32_t terminal);
    void discountWeights();
    bool supportsSubSolution(const std::vector<uint32_t>& sub_solution);
private:
    Graph& _graph;
    std::vector<Scenario>& _scenarios;
    double _lambda;
};
