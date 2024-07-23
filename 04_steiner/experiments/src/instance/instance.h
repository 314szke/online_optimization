#pragma once

#include <vector>

#include "graph.h"
#include "scenario.h"


class Instance {
public:
    void print() const;

    Graph graph;
    std::vector<Scenario> scenarios;
    std::vector<uint32_t> terminals;
};
