#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/scenario.h"


class EdgePredictor {
public:
    EdgePredictor(Graph& graph, std::vector<Scenario>& scenarios, double limit);
    void updateWeights(uint32_t terminal);
private:
    Graph& _graph;
    double edge_weight_limit;
    std::vector<Scenario>& _scenarios;
};
