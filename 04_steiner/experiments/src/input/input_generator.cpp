#include "input_generator.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>


InputGenerator::InputGenerator(const std::string& config_file) :
    config(config_file)
{}

void InputGenerator::generate(Instance& instance)
{
    generateGraph(instance);
    generateScenarios(instance);
}

void InputGenerator::generateGraph(Instance& instance)
{
    std::mt19937 engine(config.random_seed);
    std::uniform_real_distribution<double> position(0, config.dimension_size);

    std::vector<double> x(config.nb_vertices);
    std::vector<double> y(config.nb_vertices);

    for (uint32_t idx = 0; idx < config.nb_vertices; idx++) {
        x[idx] = position(engine);
        y[idx] = position(engine);
    }

    double cost;
    instance.graph.initialize(config.nb_vertices);
    for (uint32_t i = 0; i < config.nb_vertices; i++) {
        for (uint32_t j = (i+1); j < config.nb_vertices; j++) {
            cost = std::sqrt(std::pow((x[i]-x[j]), 2) + std::pow((y[i]-y[j]), 2));
            instance.graph.setCost(i,j,cost);
        }
    }

    // Vertex 0 is root
    instance.graph.activate(0);
}

void InputGenerator::generateScenarios(Instance& instance) {
    std::mt19937 engine(config.random_seed);
    std::uniform_int_distribution<uint32_t> nb_terminals(config.min_terminals, config.max_terminals);

    // Vertex 0 is root
    std::vector<uint32_t> every_terminal(config.nb_vertices - 1);
    for (uint32_t idx = 1; idx < config.nb_vertices; idx++) {
        every_terminal[idx - 1] = idx;
    }

    uint32_t n;
    std::vector<uint32_t> terminals;
    for (uint32_t it = 0; it < config.nb_scenarios; it++) {
        terminals = every_terminal;
        std::shuffle(terminals.begin(), terminals.end(), engine);
        n = nb_terminals(engine);
        terminals.resize(n);

        Scenario new_scenario(terminals, (1.0 / config.nb_scenarios));
        bool is_new = true;
        for (uint32_t idx = 0; idx < instance.scenarios.size(); idx++) {
            if (new_scenario == instance.scenarios[idx]) {
                is_new = false;
                instance.scenarios[idx] += new_scenario;
                break;
            }
        }
        if (is_new) {
            instance.scenarios.push_back(new_scenario);
        }
    }
}
