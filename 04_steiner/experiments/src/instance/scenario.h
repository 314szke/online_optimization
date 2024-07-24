#pragma once

#include <cstdint>
#include <vector>

#include "instance/graph.h"


class Scenario {
public:
    Scenario();
    Scenario(double prob, const std::vector<uint32_t>& terminal_list, std::vector<uint32_t> solution);

    void checkIfStillFeasible(uint32_t current_terminal);
    bool containsEdge(uint32_t e) const;
    void print(uint32_t idx, const Graph& graph) const;

    uint32_t occurrence;
    double probability;
    std::vector<uint32_t> terminals;
    std::vector<uint32_t> solution;

    bool feasible;
};
