#pragma once

#include <cstdint>
#include <vector>


class Path {
public:
    Path();
    void print() const;

    double cost;
    double weighted_cost;
    std::vector<uint32_t> vertices;
};
