#pragma once

#include <vector>

#include "graph.h"
#include "scenario.h"


class Instance {
public:
    uint32_t nb_terminals;
    Graph graph;
    std::vector<Scenario> scenarios;
};
