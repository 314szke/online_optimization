#pragma once

#include <string>

#include "base/configured_object.h"


class InputGenerator : private ConfiguredObject {
public:
    InputGenerator(const std::string& config_file);
    void generate();
private:
    void generateModel();
    void generateExperts();

    uint32_t nb_variables;
    uint32_t nb_constraints;
    uint32_t minimization;
    uint32_t covering;
    double min_cost;
    double max_cost;
    double min_coefficient;
    double max_coefficient;
    uint32_t min_nb_zero_coefficient;
    uint32_t max_nb_zero_coefficient;
    double min_bound;
    double max_bound;
    uint32_t nb_experts;
    uint32_t nb_perfect_expert;
    uint32_t nb_online_expert;
    uint32_t nb_adversary_expert;
    uint32_t nb_random_expert;
    double min_online_error;
    double max_online_error;
    double min_random_increase;
    double max_random_increase;
};
