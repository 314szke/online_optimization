#include "grid_generator.h"

#include <fstream>
#include <random>


GridGenerator::GridGenerator(const ArgumentParser& arg_parser) :
    _arg_parser(arg_parser),
    width(0),
    height(0),
    nb_requests(0),
    edge_prob(0.0),
    min_coeff(0.0),
    max_coeff(0.0),
    min_degree(0.0),
    max_degree(0.0),
    min_constant(0.0),
    max_constant(0.0),
    nb_vertices(0),
    nb_edges(0),
    max_degree_found(0.0),
    engine(rd())
{
    f_in.open(_arg_parser.generator_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.generator_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(width);
    readParameter(height);
    readParameter(nb_requests);
    readParameter(edge_prob);
    readParameter(min_coeff);
    readParameter(max_coeff);
    readParameter(min_degree);
    readParameter(max_degree);
    readParameter(min_constant);
    readParameter(max_constant);

    nb_vertices = width * height;
    nb_edges = 0;

    // Keep track of connections to avoid unreachable regions
    in_degrees.resize(nb_vertices, 4);
    out_degrees.resize(nb_vertices, 4);
    // Side vertices have less connections
    for (uint32_t idx = 0; idx < width; idx++) {
        // top vertices
        in_degrees[idx] = 3;
        out_degrees[idx] = 3;
        // bottom vertices
        in_degrees[(((height - 1) * width) + idx)] = 3;
        out_degrees[(((height - 1) * width) + idx)] = 3;
    }
    for (uint32_t idx = 0; idx < height; idx++) {
        // left vertices
        in_degrees[(idx * width)] = 3;
        out_degrees[(idx * width)] = 3;
        //right vertices
        in_degrees[((idx * width) + (width - 1))] = 3;
        out_degrees[((idx * width) + (width - 1))] = 3;
    }
    // Corners have even less
    in_degrees[0] = 2;
    out_degrees[0] = 2;
    in_degrees[(width - 1)] = 2;
    out_degrees[(width - 1)] = 2;
    in_degrees[((height - 1) * width)] = 2;
    out_degrees[((height - 1) * width)] = 2;
    in_degrees[(nb_vertices - 1)] = 2;
    out_degrees[(nb_vertices - 1)] = 2;


    // Random generators
    edge_dist.reset(new DoubleDist_t(0.0, 1.0));
    coeff_dist.reset(new DoubleDist_t(min_coeff, max_coeff));
    degree_dist.reset(new DoubleDist_t(min_degree, max_degree));
    constant_dist.reset(new DoubleDist_t(min_constant, max_constant));
    request_dist.reset(new UIntDist_t(0, (nb_vertices - 1)));

    f_in.close();
}

void GridGenerator::generate()
{
    generateData();
    generateConfig();
}

void GridGenerator::generateData()
{
    std::ofstream f_out(_arg_parser.data_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.data_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << nb_vertices << std::endl;
    f_out << 0 << "        " << std::endl;

    uint32_t v1, v2;

    // Horizontal edges
    for (uint32_t h_idx = 0; h_idx < height; h_idx++) {
        for (uint32_t w_idx = 0; w_idx < (width - 1); w_idx++) {
            v1 = (h_idx * width) + w_idx;
            v2 = v1 + 1;

            createEdge(v1, v2, f_out);
            createEdge(v2, v1, f_out);
        }
    }

    // Vertical edges
    for (uint32_t w_idx = 0; w_idx < width; w_idx++) {
        for (uint32_t h_idx = 0; h_idx < (height - 1); h_idx++) {
            v1 = (h_idx * width) + w_idx;
            v2 = ((h_idx + 1) * width) + w_idx;

            createEdge(v1, v2, f_out);
            createEdge(v2, v1, f_out);
        }
    }

    f_out << nb_requests << std::endl;

    uint32_t i, j;
    for (uint32_t r = 0; r < nb_requests; r++) {
        i = (*request_dist)(engine);
        j = (*request_dist)(engine);
        while (i == j) {
            i = (*request_dist)(engine);
            j = (*request_dist)(engine);
        }
        f_out << i << " - " << j << std::endl;
    }

    f_out.seekp(0);
    f_out << nb_vertices << std::endl;
    f_out << nb_edges;

    f_out.close();
}

void GridGenerator::generateConfig()
{
    std::ofstream f_out(_arg_parser.config_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.config_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << "time_horizon = 60          # number of iterations in the Frank-Wolfe algorithm" << std::endl;
    f_out << "max_search_iter = 15       # the maximum number of eta search steps in the Frank-Wolfe algorithm" << std::endl;
    f_out << "max_distance = 0.001       # maximum distance between x and v before terminating the Frank-Wolfe algorithm" << std::endl;
    f_out << "gurobi_verbosity = 0       # if set to 1 gurobi will print a lot of messages" << std::endl;
    f_out << "epsilon = 0.0001           # variables need to reach (1-eps) value to be selected in the solver" << std::endl;
    f_out << "cost_degree = " << max_degree_found << "             # the maximum degree of the polynomial used for edge costs" << std::endl;
    f_out << "random_iteration = 10      # number of random iterations to estimate the value of the partial derivative of F(x)" << std::endl;
    f_out << "DT = 0.001                 # the time step for the increasing rate in solver" << std::endl;
    f_out << "eta_step = 0.099           # the step size in the range of (0, 1] for eta in the experiments" << std::endl;
    f_out << "nb_oracles = 3             # the algorithm's performance will be compared to the this many oracles that have randomly rounded solutions" << std::endl;
    f_out << "nb_generated_oracles = 20  # the number of oracles generated and then the nb_oracles amount is kept" << std::endl;
    f_out << "nb_oracle_search = 10      # max number of attempts to look for a unique randomly rounded oracle" << std::endl;
    f_out << "random_seed = 1234         # used during oracle prediction generation" << std::endl;
    f_out << "use_threads = 0            # value 0 or 1 to indicate whether multiple threads should be used during the execution" << std::endl;

    f_out.close();
}

void GridGenerator::createEdge(uint32_t v1 , uint32_t v2, std::ofstream& f_out)
{
    double p = (*edge_dist)(engine);
    if (edgeShouldBeCreated(p, v1, v2)) {
        double coeff = (*coeff_dist)(engine);
        double degree = (*degree_dist)(engine);
        double constant = (*constant_dist)(engine);

        f_out << v1 << " - " << v2 << " # " << coeff << " # " << degree << " # " << constant << std::endl;
        nb_edges++;

        if (degree > max_degree_found) {
            max_degree_found = degree;
        }
    }
}

bool GridGenerator::edgeShouldBeCreated(double probability, uint32_t v1, uint32_t v2)
{
    if (out_degrees[v1] == 1)
        return true;

    if (in_degrees[v2] == 1)
        return true;

    if (probability < edge_prob)
        return true;

    out_degrees[v1]--;
    in_degrees[v2]--;

    return false;
}
