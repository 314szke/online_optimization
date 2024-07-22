#pragma once

#include <cstdint>
#include <vector>


class Scenario {
public:
    Scenario();
    Scenario(double prob, const std::vector<uint32_t>& terminal_list, std::vector<uint32_t> solution);

    uint32_t occurrence;
    double probability;
    std::vector<uint32_t> terminals;
    std::vector<uint32_t> solution;
};
