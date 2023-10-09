#include "connected_graph_generator.h"

#include <fstream>
#include <random>


ConnectedGraphGenerator::ConnectedGraphGenerator(const ArgumentParser& arg_parser) :
    _arg_parser(arg_parser),
    nb_vertices(0),
    nb_edges(0),
    nb_requests(0),
    min_coeff(0.0),
    max_coeff(0.0),
    min_degree(0.0),
    max_degree(0.0),
    min_constant(0.0),
    max_constant(0.0),
    max_degree_found(0.0),
    engine(rd())
{
    f_in.open(_arg_parser.generator_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.generator_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(nb_vertices);
    readParameter(nb_edges);
    readParameter(nb_requests);
    readParameter(min_coeff);
    readParameter(max_coeff);
    readParameter(min_degree);
    readParameter(max_degree);
    readParameter(min_constant);
    readParameter(max_constant);

    edge_matrix.resize(nb_vertices, BoolVec_t(nb_vertices, false));

    // Random generators
    coeff_dist.reset(new DoubleDist_t(min_coeff, max_coeff));
    degree_dist.reset(new DoubleDist_t(min_degree, max_degree));
    constant_dist.reset(new DoubleDist_t(min_constant, max_constant));
    vertex_dist.reset(new UIntDist_t(0, (nb_vertices - 1)));

    f_in.close();
}

void ConnectedGraphGenerator::generate()
{
    generateData();
    generateConfig();
}

void ConnectedGraphGenerator::generateData()
{
    std::ofstream f_out(_arg_parser.data_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.data_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << nb_vertices << std::endl;
    f_out << (nb_edges + 2 * nb_vertices)  << std::endl;

    uint32_t v1, v2;

    // Create a cycle that connects every edge
    for (v1 = 0; v1 < nb_vertices; v1++) {
        v2 = next(v1);
        createEdge(v1, v2, f_out);
        createEdge(v2, v1, f_out);
    }

    // Create random edges until the requested number of edges is reached
    for (uint32_t it = 0; it < nb_edges; it++) {
        do {
            v1 = (*vertex_dist)(engine);
            v2 = (*vertex_dist)(engine);
        } while ((v1 == v2) || edge_matrix[v1][v2] || (v2 == next(v1)) || (v1 == next(v2)));

        createEdge(v1, v2, f_out);
    }

    f_out << nb_requests << std::endl;

    uint32_t i, j;
    for (uint32_t r = 0; r < nb_requests; r++) {
        do {
            i = (*vertex_dist)(engine);
            j = (*vertex_dist)(engine);
        } while (i == j);
        f_out << i << " - " << j << std::endl;
    }

    f_out.close();
}

void ConnectedGraphGenerator::generateConfig()
{
    std::ofstream f_out(_arg_parser.config_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.config_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << "time_horizon = 600         # number of iterations in the Frank-Wolfe algorithm" << std::endl;
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

void ConnectedGraphGenerator::createEdge(uint32_t v1 , uint32_t v2, std::ofstream& f_out)
{
    double coeff = (*coeff_dist)(engine);
    double degree = (*degree_dist)(engine);
    double constant = (*constant_dist)(engine);

    f_out << v1 << " - " << v2 << " # " << coeff << " # " << degree << " # " << constant << std::endl;
    edge_matrix[v1][v2] = true;

    if (degree > max_degree_found) {
        max_degree_found = degree;
    }
}

uint32_t ConnectedGraphGenerator::next(uint32_t v) {
    if ((v + 1) < nb_vertices) {
        return (v + 1);
    }
    return 0;
}
