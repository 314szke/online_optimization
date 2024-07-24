#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"


class OfflineSolver {
public:
    OfflineSolver(const Graph& graph, const std::vector<uint32_t>& terminals);
    void print() const;
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph _graph;
    std::vector<uint32_t> solution;
};
