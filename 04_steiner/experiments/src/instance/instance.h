#pragma once

#include <vector>

#include "graph.h"
#include "scenario.h"


class Instance {
public:
    Instance();
    void print() const;

    uint32_t time;
    Graph graph;
    std::vector<Scenario> scenarios;
    std::vector<uint32_t> terminals;
};
