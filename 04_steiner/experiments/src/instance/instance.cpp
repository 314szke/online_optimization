#include "instance.h"


Instance::Instance()
{
    terminals = {0}; // root is vertex 0
}

void Instance::print() const
{
    for (uint32_t idx = 0; idx < scenarios.size(); idx++) {
        scenarios[idx].print((idx+1), graph);
    }
}
