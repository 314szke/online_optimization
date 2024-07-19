#pragma once

#include <string>

#include "config.h"
#include "instance/instance.h"


class InputGenerator {
public:
    InputGenerator(const std::string& config_file);
    void generate(Instance& instance);
private:
    void generateGraph(Instance& instance);
    void generateScenarios(Instance& instance);

    Config config;
};
