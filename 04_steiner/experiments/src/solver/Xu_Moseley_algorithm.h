#pragma once

#include <cstdint>
#include <vector>

#include "algorithm/mcp.h"
#include "instance/graph.h"
#include "prediction/vertex_predictor.h"


class XuMoseleyAlgorithm {
public:
    XuMoseleyAlgorithm(const Graph& graph, const VertexPredictor& predictor);
    void print() const;

    void connectTerminal(uint32_t t);
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph _graph;
    const VertexPredictor& _predictor;
    MinCostPathFinder MCPF;
    std::vector<uint32_t> solution;
};
