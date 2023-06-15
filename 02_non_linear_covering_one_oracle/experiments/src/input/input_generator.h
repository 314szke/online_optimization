#pragma once

#include<fstream>
#include<regex>
#include<string>

#include "config/argument_parser.h"


class InputGenerator {
public:
    InputGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    template<typename T>
    void readParameter(T& parameter);

    uint32_t nb_vertices;
    uint32_t nb_edges;
    uint32_t nb_requests;
    double edge_prob;
    uint32_t min_degree;
    uint32_t max_degree;
    double min_coeff;
    double max_coeff;

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
    std::regex config_pattern;
    std::smatch matches;
    std::ssub_match match;
};
