#pragma once

#include <fstream>
#include <regex>
#include <string>


class Config {
public:
    Config(const std::string& config_file);

    uint32_t nb_vertices;
    uint32_t nb_terminals;
    uint32_t nb_scenarios;
    uint32_t dimension_size;
    double lambda;
    uint32_t random_seed;
    uint32_t verbosity;
private:
    template<typename T>
    void readParameter(T& parameter);

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;

    std::regex config_pattern;
    std::smatch matches;
    std::ssub_match match;
};
