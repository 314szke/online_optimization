#pragma once

#include "base/configured_object.h"
#include "config/argument_parser.h"


class GridGenerator : private ConfiguredObject {
public:
    GridGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateData();
    void generateConfig();

    const ArgumentParser& _arg_parser;

    uint32_t width;
    uint32_t height;
    uint32_t nb_requests;
    double min_degree;
    double max_degree;
    double min_coeff;
    double max_coeff;

    uint32_t nb_vertices;
    uint32_t nb_edges;
    double max_degree_found;
};
