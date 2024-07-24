#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/path.h"



class MinCostPathFinder {
public:
    MinCostPathFinder(const Graph& graph);

    static uint32_t MinCostEdge(const Graph& graph, uint32_t terminal);

    Path MinCostWeightedPath(uint32_t terminal);
    Path MinCostWeightedConditionalPath(uint32_t terminal, const std::vector<uint32_t>& condition);
private:
    void initialize(uint32_t terminal, const std::vector<uint32_t>& condition);
    bool edgeSatisfiesCondition(uint32_t edge, const std::vector<uint32_t>& condition);
    uint32_t getClosestUnvisitedVertex();
    void updateCosts(uint32_t vertex, const std::vector<uint32_t>& condition);
    uint32_t getClosestActiveVertex(uint32_t terminal);
    Path buildPath(uint32_t terminal, uint32_t vertex);

    const Graph& _graph;
    std::vector<double> costs;
    std::vector<int> arrived_from;
    std::vector<int> Q;
};
