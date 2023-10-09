#pragma once

#include <memory>
#include <random>
#include <vector>

#include "base/configured_object.h"
#include "config/argument_parser.h"


class ConnectedGraphGenerator : private ConfiguredObject {
public:
    ConnectedGraphGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateData();
    void generateConfig();
    void createEdge(uint32_t v1 , uint32_t v2, std::ofstream& f_out);
    uint32_t next(uint32_t v);

    const ArgumentParser& _arg_parser;

    uint32_t nb_vertices;
    uint32_t nb_edges;
    uint32_t nb_requests;
    double min_coeff;
    double max_coeff;
    double min_degree;
    double max_degree;
    double min_constant;
    double max_constant;

    double max_degree_found;
    typedef std::vector<bool> BoolVec_t;
    std::vector<BoolVec_t> edge_matrix;

    typedef std::uniform_real_distribution<double> DoubleDist_t;
    typedef std::uniform_int_distribution<uint32_t> UIntDist_t;

    std::random_device rd;
    std::mt19937 engine;
    std::unique_ptr<DoubleDist_t> coeff_dist;
    std::unique_ptr<DoubleDist_t> degree_dist;
    std::unique_ptr<DoubleDist_t> constant_dist;
    std::unique_ptr<UIntDist_t> vertex_dist;
};
