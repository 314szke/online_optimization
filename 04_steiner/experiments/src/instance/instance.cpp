#include "instance.h"

#include "visualization/print.hpp"


Instance::Instance() :
    time(0)
{}

void Instance::print() const
{
    for (uint32_t idx = 0; idx < scenarios.size(); idx++) {
        scenarios[idx].print((idx+1), graph);
    }

    print_vector("Terminals", terminals);
}
