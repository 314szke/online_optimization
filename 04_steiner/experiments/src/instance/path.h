#pragma once

#include <cstdint>
#include <vector>


class Path {
public:
    Path();

    double cost;
    double weighted_cost;
    std::vector<uint32_t> vertices;
};
