#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"


class GreedySolver {
public:
    GreedySolver(const Graph& graph);
    void print(uint32_t verbosity) const;

    void connectTerminal(uint32_t t);
    const std::vector<uint32_t>& getSolution() const;
private:
    Graph _graph;
    std::vector<uint32_t> solution;
};
