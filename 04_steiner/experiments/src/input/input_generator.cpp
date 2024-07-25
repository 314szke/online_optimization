#include "input_generator.h"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <random>
#include <vector>

#include "algorithm/mst.h"


#define MAX_BITSET_LENGTH 200


InputGenerator::InputGenerator(const Config& config) :
    _config(config)
{}

void InputGenerator::generate(Instance& instance)
{
    generateGraph(instance);
    //generateUniformScenarios(instance);
    generateNormalScenarios(instance);
    generateTerminals(instance);
}

void InputGenerator::generateGraph(Instance& instance)
{
    std::mt19937 engine(_config.random_seed);
    std::uniform_real_distribution<double> position(0, _config.dimension_size);

    std::vector<double> x(_config.nb_vertices);
    std::vector<double> y(_config.nb_vertices);

    for (uint32_t idx = 0; idx < _config.nb_vertices; idx++) {
        x[idx] = position(engine);
        y[idx] = position(engine);
    }

    double cost;
    instance.graph.initialize(_config.nb_vertices);
    for (uint32_t i = 0; i < _config.nb_vertices; i++) {
        for (uint32_t j = (i+1); j < _config.nb_vertices; j++) {
            cost = std::sqrt(std::pow((x[i]-x[j]), 2) + std::pow((y[i]-y[j]), 2));
            instance.graph.setCost(i,j,cost);
        }
    }

    // Vertex 0 is root
    instance.graph.activate(0);
}

void InputGenerator::generateUniformScenarios(Instance& instance) {
    uint32_t max_value = static_cast<uint32_t>(std::pow(2, (_config.nb_vertices - 1)) - 1);
    std::mt19937 engine(_config.random_seed + 1);
    std::uniform_int_distribution<uint32_t> random_bit_string(1, max_value);

    double probability = (1.0 / _config.nb_scenarios);
    for (uint32_t it = 0; it < _config.nb_scenarios; it++) {
        std::bitset<MAX_BITSET_LENGTH> terminal_string(random_bit_string(engine));
        std::vector<uint32_t> terminals;

        for (uint32_t idx = 0; idx < (_config.nb_vertices - 1); idx++) {
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
            std::vector<uint32_t> solution = MST(instance.graph, terminals);
            double cost = instance.graph.getSteinerCost(solution);
            instance.scenarios.push_back(Scenario(cost, probability, terminals, solution));
        }
    }
}

void InputGenerator::generateNormalScenarios(Instance& instance)
{
    std::mt19937 engine(_config.random_seed + 1);
    std::uniform_real_distribution<double> random_bit(0.0, 1.0);

    double probability = (1.0 / _config.nb_scenarios);
    for (uint32_t it = 0; it < _config.nb_scenarios; it++) {
        std::vector<uint32_t> terminals;
        for (uint32_t idx = 0; idx < (_config.nb_vertices - 1); idx++) {
            if (random_bit(engine) > 0.5) {
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
            std::vector<uint32_t> solution = MST(instance.graph, terminals);
            double cost = instance.graph.getSteinerCost(solution);
            instance.scenarios.push_back(Scenario(cost, probability, terminals, solution));
        }
    }
}

void InputGenerator::generateTerminals(Instance& instance)
{
    std::mt19937 engine(_config.random_seed + 2);
    std::vector<uint32_t> terminals((_config.nb_vertices - 1), 0);
    for (uint32_t idx = 0; idx < _config.nb_terminals; idx++) {
        terminals[idx] = 1;
    }
    std::shuffle(terminals.begin(), terminals.end(), engine);

    for (uint32_t idx = 0; idx < (_config.nb_vertices - 1); idx++) {
        if (terminals[idx] == 1) {
            instance.terminals.push_back(idx + 1);
        }
    }
    std::shuffle(instance.terminals.begin(), instance.terminals.end(), engine);
}
