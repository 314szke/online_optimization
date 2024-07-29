#pragma once

#include <cstdint>
#include <vector>

#include "algorithm/mcp.h"
#include "instance/graph.h"


class WeightedGreedySolver {
public:
    WeightedGreedySolver(Graph& graph);
    void print(uint32_t verbosity) const;

    void connectTerminal(uint32_t t);
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph& _graph;
    MinCostPathFinder MCPF;
    std::vector<uint32_t> solution;
};
