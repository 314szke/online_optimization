#pragma once

#include <cstdint>
#include <set>
#include <vector>


class Scenario {
public:
    Scenario();
    Scenario(const std::vector<uint32_t>& terminal_list, double prob);

    Scenario& operator+=(const Scenario& other) {
        this->probability += other.probability;
        this->occurrence += other.occurrence;
        return *this;
    }

    friend bool operator==(const Scenario& left, const Scenario& right) {
        std::set<uint32_t> left_set(left.terminals.begin(), left.terminals.end());
        std::set<uint32_t> right_set(right.terminals.begin(), right.terminals.end());
        return left_set == right_set;
    }
private:
    uint32_t occurrence;
    double probability;
    std::vector<uint32_t> terminals;
    std::vector<uint32_t> solution;
};
