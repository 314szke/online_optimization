#include "instance.h"

#include "visualization/print.hpp"


Instance::Instance() :
    time(0)
{}

bool Instance::isRunning() const
{
    return (time < terminals.size());
}

uint32_t Instance::getNextTerminal()
{
    time++;
    return terminals[(time - 1)];
}

void Instance::print() const
{
    for (uint32_t idx = 0; idx < scenarios.size(); idx++) {
        scenarios[idx].print((idx+1), graph);
    }

    print_vector("Terminals", terminals);
}
