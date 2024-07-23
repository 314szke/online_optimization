#include "input_generator.h"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <random>
#include <vector>

#include "algorithm/mst.h"


#define MAX_BITSET_LENGTH 200


InputGenerator::InputGenerator(const std::string& config_file) :
    config(config_file)
{}

void InputGenerator::generate(Instance& instance)
{
    generateGraph(instance);
    generateScenarios(instance);
    generateTerminals(instance);
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
    uint32_t max_value = static_cast<uint32_t>(std::pow(2, (config.nb_vertices - 1)) - 1);
    std::mt19937 engine(config.random_seed + 1);
    std::uniform_int_distribution<uint32_t> random_bit_string(1, max_value);

    double probability = (1.0 / config.nb_scenarios);
    for (uint32_t it = 0; it < config.nb_scenarios; it++) {
        std::bitset<MAX_BITSET_LENGTH> terminal_string(random_bit_string(engine));
        std::vector<uint32_t> terminals;

        for (uint32_t idx = 0; idx < (config.nb_vertices - 1); idx++) {
            if (terminal_string[idx]) {
                terminals.push_back(idx + 1);
            }
        }

        bool is_new = true;
        for (uint32_t idx = 0; idx < instance.scenarios.size(); idx++) {
            if (terminals == instance.scenarios[idx].terminals) {
                is_new = false;
                instance.scenarios[idx].occurrence += 1;
                instance.scenarios[idx].probability += probability;
                break;
            }
        }
        if (is_new) {
            instance.scenarios.push_back(Scenario(probability, terminals, MST(instance.graph, terminals)));
        }
    }
}

void InputGenerator::generateTerminals(Instance& instance)
{
    uint32_t max_value = static_cast<uint32_t>(std::pow(2, (config.nb_vertices - 1)) - 1);
    std::mt19937 engine(config.random_seed + 2);
    std::uniform_int_distribution<uint32_t> random_bit_string(1, max_value);
    std::bitset<MAX_BITSET_LENGTH> terminal_string(random_bit_string(engine));

    for (uint32_t idx = 0; idx < (config.nb_vertices - 1); idx++) {
        if (terminal_string[idx]) {
            instance.terminals.push_back(idx + 1);
        }
    }
}
