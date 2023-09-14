#pragma once

#include "base/configured_object.h"
#include "config/argument_parser.h"


class RandomGenerator : private ConfiguredObject {
public:
    RandomGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateData();
    void generateConfig();

    const ArgumentParser& _arg_parser;

    uint32_t nb_vertices;
    uint32_t nb_edges;
    uint32_t nb_requests;
    double edge_prob;
    double min_coeff;
    double max_coeff;
    double min_degree;
    double max_degree;
    double min_constant;
    double max_constant;

    double max_degree_found;
};
