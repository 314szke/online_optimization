#pragma once

#include<fstream>
#include<regex>
#include<string>

#include "base/configured_object.h"
#include "config/argument_parser.h"


class InputGenerator : private ConfiguredObject {
public:
    InputGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateData();
    void generateConfig();

    const ArgumentParser& _arg_parser;

    uint32_t nb_vertices;
    uint32_t nb_edges;
    uint32_t nb_requests;
    double edge_prob;
    uint32_t min_degree;
    uint32_t max_degree;
    double min_coeff;
    double max_coeff;
};
