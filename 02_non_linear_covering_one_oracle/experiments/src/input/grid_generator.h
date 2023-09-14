#pragma once

#include <memory>
#include <random>
#include <vector>

#include "base/configured_object.h"
#include "config/argument_parser.h"


class GridGenerator : private ConfiguredObject {
public:
    GridGenerator(const ArgumentParser& arg_parser);
    void generate();
private:
    void generateData();
    void generateConfig();
    void createEdge(uint32_t v1 , uint32_t v2, std::ofstream& f_out);
    bool edgeShouldBeCreated(double probability, uint32_t v1, uint32_t v2);

    const ArgumentParser& _arg_parser;

    uint32_t width;
    uint32_t height;
    uint32_t nb_requests;
    double edge_prob;
    double min_coeff;
    double max_coeff;
    double min_degree;
    double max_degree;
    double min_constant;
    double max_constant;

    uint32_t nb_vertices;
    uint32_t nb_edges;
    double max_degree_found;

    std::vector<uint8_t> in_degrees;
    std::vector<uint8_t> out_degrees;

    typedef std::uniform_real_distribution<double> DoubleDist_t;
    typedef std::uniform_int_distribution<uint32_t> UIntDist_t;

    std::random_device rd;
    std::mt19937 engine;
    std::unique_ptr<DoubleDist_t> edge_dist;
    std::unique_ptr<DoubleDist_t> coeff_dist;
    std::unique_ptr<DoubleDist_t> degree_dist;
    std::unique_ptr<DoubleDist_t> constant_dist;
    std::unique_ptr<UIntDist_t> request_dist;
};
