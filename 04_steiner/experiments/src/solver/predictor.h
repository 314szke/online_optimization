#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/scenario.h"


class Predictor {
public:
    Predictor(Graph& graph, std::vector<Scenario>& scenarios);
    void updateWeights(uint32_t terminal);
private:
    Graph& _graph;
    std::vector<Scenario>& _scenarios;
};
