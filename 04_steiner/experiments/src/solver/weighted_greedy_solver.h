#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"


class WeightedGreedySolver {
public:
    WeightedGreedySolver(Graph& graph);
    void print() const;

    void connectTerminal(uint32_t t);
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph& _graph;
    std::vector<uint32_t> solution;
};
