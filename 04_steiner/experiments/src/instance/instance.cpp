#include "instance.h"


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

void Instance::print(uint32_t verbosity) const
{
    for (uint32_t idx = 0; idx < scenarios.size(); idx++) {
        if (scenarios[idx].feasible) {
            scenarios[idx].print((idx+1), graph, verbosity);
        }
    }
}
