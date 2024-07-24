#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/scenario.h"


class Predictor {
public:
    Predictor(Graph& graph, std::vector<Scenario>& scenarios);
    void updateWeights(const std::vector<uint32_t>& terminals);
private:
    Graph& _graph;
    std::vector<Scenario>& _scenarios;
};
