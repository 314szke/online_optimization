#pragma once

#include <cstdint>
#include <vector>

#include "algorithm/mcp.h"
#include "instance/graph.h"
#include "prediction/edge_predictor.h"


class WeightedGreedySolver {
public:
    WeightedGreedySolver(Graph& graph, EdgePredictor& predictor);
    void print(uint32_t verbosity) const;

    void connectTerminal(uint32_t t);
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph& _graph;
    EdgePredictor& _predictor;
    MinCostPathFinder MCPF;
    std::vector<uint32_t> solution;
};
