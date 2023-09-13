#include "input/random_generator.h"

#include <fstream>
#include <random>


RandomGenerator::RandomGenerator(const ArgumentParser& arg_parser) :
    _arg_parser(arg_parser),
    nb_vertices(0),
    nb_edges(0),
    nb_requests(0),
    edge_prob(0.0),
    min_degree(0),
    max_degree(0),
    min_coeff(0.0),
    max_coeff(0.0),
    max_degree_found(0.0)
{
    f_in.open(_arg_parser.generator_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.generator_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(nb_vertices);
    readParameter(nb_requests);
    readParameter(edge_prob);
    readParameter(min_degree);
    readParameter(max_degree);
    readParameter(min_coeff);
    readParameter(max_coeff);

    f_in.close();
}

void RandomGenerator::generate()
{
    generateData();
    generateConfig();
}

void RandomGenerator::generateData()
{
    std::ofstream f_out(_arg_parser.data_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.data_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << nb_vertices << std::endl;
    f_out << 0 << "        " << std::endl;

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> edge_dist(0.0, 1.0);
    std::uniform_real_distribution<double> degree_dist(min_degree, max_degree);
    std::uniform_real_distribution<double> coeff_dist(min_coeff, max_coeff);
    std::uniform_int_distribution<uint32_t> request_dist(0, (nb_vertices - 1));

    double coeff, degree;

    for (uint32_t i = 0; i < nb_vertices; i++) {
        for (uint32_t j = 0; j < nb_vertices; j++) {
            if (i == j) {
                continue;
            }

            coeff = edge_dist(engine);
            if (coeff < edge_prob) {
                nb_edges++;

                f_out << i << " - " << j << " # ";
                coeff = coeff_dist(engine);
                degree = degree_dist(engine);
                f_out << coeff << " # " << degree << " # 0" << std::endl;

                if (degree > max_degree_found) {
                    max_degree_found = degree;
                }
            }
        }
    }

    f_out << nb_requests << std::endl;

    uint32_t i, j;
    for (uint32_t r = 0; r < nb_requests; r++) {
        i = request_dist(engine);
        j = request_dist(engine);
        while (i == j) {
            i = request_dist(engine);
            j = request_dist(engine);
        }
        f_out << i << " - " << j << std::endl;
    }

    f_out.seekp(0);
    f_out << nb_vertices << std::endl;
    f_out << nb_edges;

    f_out.close();
}

void RandomGenerator::generateConfig()
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
    f_out << "nb_oracle_search = 10      # max number of attempts to look for a unique randomly rounded oracle" << std::endl;
    f_out << "random_seed = 1234         # used during oracle prediction generation" << std::endl;

    f_out.close();
}
