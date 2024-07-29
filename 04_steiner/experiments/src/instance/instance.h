#pragma once

#include <cstdint>
#include <vector>

#include "graph.h"
#include "scenario.h"


class Instance {
public:
    Instance();

    bool isRunning() const;
    uint32_t getNextTerminal();
    void print(uint32_t verbosity) const;

    uint32_t time;
    Graph graph;

    std::vector<Scenario> scenarios;
    std::vector<uint32_t> terminals;
};
