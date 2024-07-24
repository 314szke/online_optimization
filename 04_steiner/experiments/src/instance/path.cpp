#include "path.h"

#include <iostream>


Path::Path() :
    cost(0),
    weighted_cost(0)
{}

void Path::print() const
{
    std::cout << "Path (" << cost << ", " << weighted_cost << ") : [";
    for (uint32_t idx = 0; idx < (vertices.size() - 1); idx++) {
        std::cout << (vertices[idx] + 1) << ", ";
    }
    std::cout << (vertices[(vertices.size() - 1)] + 1) << "]" << std::endl;
}
