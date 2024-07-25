#pragma once

#include <string>

#include "config.h"
#include "instance/instance.h"


class InputGenerator {
public:
    InputGenerator(const Config& config);
    void generate(Instance& instance);
private:
    void generateGraph(Instance& instance);
    void generateUniformScenarios(Instance& instance);
    void generateNormalScenarios(Instance& instance);
    void generateTerminals(Instance& instance);

    const Config& _config;
};
