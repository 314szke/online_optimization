#include "input/input_generator.h"

#include<fstream>
#include<random>

static const std::string OUTPUT_FILENAME = "new_input.lp";


InputGenerator::InputGenerator(const ArgumentParser& arg_parser) :
    nb_vertices(0),
    nb_edges(0),
    nb_requests(0),
    edge_prob(0.0),
    min_degree(0),
    max_degree(0),
    min_coeff(0.0),
    max_coeff(0.0),
    line_counter(0),
    config_pattern("[a-zA-Z_]+\\s+=\\s+([0-9\\.]*)\\s+[\\S\\s]*")
{
    f_in.open(arg_parser.config_file);
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << arg_parser.config_file << " could not be open!" << std::endl;
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

void InputGenerator::generate()
{

    std::ofstream f_out(OUTPUT_FILENAME);
    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << OUTPUT_FILENAME << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << nb_vertices << std::endl;
    f_out << 0 << "        " << std::endl;

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> edge_dist(0.0, 1.0);
    std::uniform_int_distribution<uint32_t> degree_dist(min_degree, max_degree);
    std::uniform_real_distribution<double> coeff_dist(min_coeff, max_coeff);
    std::uniform_int_distribution<uint32_t> request_dist(0, (nb_vertices - 1));

    double random_double;
    uint32_t random_uint;

    for (uint32_t i = 0; i < nb_vertices; i++) {
        for (uint32_t j = 0; j < nb_vertices; j++) {
            if (i == j) {
                continue;
            }

            random_double = edge_dist(engine);
            if (random_double < edge_prob) {
                nb_edges++;

                f_out << i << " - " << j << " # ";
                random_double = coeff_dist(engine);
                random_uint = degree_dist(engine);
                f_out << random_double << " * x^" << random_uint << " # ";
                f_out << (random_double * random_uint) << " * x^" << (random_uint - 1) << std::endl;
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

template<typename T>
void InputGenerator::readParameter(T& parameter)
{
    line.resize(0);
    std::getline(f_in, line);
    line_counter++;

    if (line.empty()) {
        std::stringstream message;
        message << "ERROR: line " << line_counter << " should not be empty!" << std::endl;
        throw std::runtime_error(message.str());
    }

    if (std::regex_match(line, matches, config_pattern)) {
        match = matches[1];
        std::stringstream ss;
        ss << match.str();
        ss >> parameter;
    } else {
        std::stringstream message;
        message << "ERROR: the parameter on line " << line_counter << " has invalid format!" << std::endl;
        throw std::runtime_error(message.str());
    }
}
