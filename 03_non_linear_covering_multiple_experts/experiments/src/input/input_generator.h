#pragma once

#include <string>

#include "base/configured_object.h"
#include "config/argument_parser.h"


class InputGenerator : private ConfiguredObject {
public:
    InputGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateModel();
    void generateExperts();
    void generateConfigFile();

    const ArgumentParser& _arg_parser;

    uint32_t nb_machines;
    uint32_t nb_jobs;

    uint32_t min_processing_time;
    uint32_t max_processing_time;

    uint32_t nb_experts;
    uint32_t nb_perfect_expert;
    uint32_t nb_perturbed_expert;
    double min_perturbation_ratio;
    double max_perturbation_ratio;
    uint32_t nb_online_expert;
    uint32_t nb_adversary_expert;
    uint32_t nb_min_processing_expert;
    uint32_t nb_random_expert;
};
