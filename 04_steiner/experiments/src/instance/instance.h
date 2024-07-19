#pragma once

#include "graph.h"
#include "scenario.h"

class Instance {
public:
    std::vector<Scenario> scenarios;
    Graph graph;
};
