#include "parser.h"

#include <iostream>
#include <sstream>
#include <regex>


ModelParser::ModelParser(std::string& file_name) :
    f_in(file_name),
    line_counter(0)
{
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << file_name << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }
}

void ModelParser::parse(Model* model)
{
    uint32_t nb_vertices = readInteger();
    uint32_t nb_edges = readInteger();
    model->initialize(nb_vertices, nb_edges);

    const std::regex edge_parameters("\\s*([0-9]+)\\s*-\\s*([0-9]+)\\s*#\\s*([x0-9\\.\\*\\^\\-\\+\\/\\s]+)\\s*#\\s*([x0-9\\.\\*\\^\\-\\+\\/\\s]+)\\s*[\\S\\s]*");

    std::smatch matched_parameters;
    uint32_t i, j;
    std::ssub_match match_1;
    std::ssub_match match_2;

    // Read edge parameters
    for (uint32_t idx = 0; idx < nb_edges; idx++) {
        readLine();
        if (std::regex_match(line, matched_parameters, edge_parameters)) {
            match_1 = matched_parameters[1];
            match_2 = matched_parameters[2];

            i = stoi(match_1.str());
            j = stoi(match_2.str());

            match_1 = matched_parameters[3];
            match_2 = matched_parameters[4];

            model->addEdge(i, j, match_1.str(), match_2.str());
        } else {
            std::stringstream message;
            message << "ERROR: edge on line " << line_counter << " has invalid format!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }

    // Read request parameters
    uint32_t nb_requests = readInteger();
    const std::regex request_parameters("\\s*([0-9]+)\\s*-\\s*([0-9]+)\\s*[\\S\\s]*");

    for (uint32_t idx = 0; idx < nb_requests; idx++) {
        readLine();
        if (std::regex_match(line, matched_parameters, request_parameters)) {
            match_1 = matched_parameters[1];
            match_2 = matched_parameters[2];

            i = stoi(match_1.str());
            j = stoi(match_2.str());

            model->addRequest(i, j);
        } else {
            std::stringstream message;
            message << "ERROR: request on line " << line_counter << " has invalid format!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }

    f_in.close();
}

void ModelParser::readLine()
{
    line.resize(0);
    std::getline(f_in, line);
    line_counter++;

    if (line.empty()) {
        std::stringstream message;
        message << "ERROR: line " << line_counter << " should not be empty!" << std::endl;
        throw std::runtime_error(message.str());
    }
}

uint32_t ModelParser::readInteger()
{
    readLine();

    const std::regex integer_pattern("\\s*([0-9]+)\\s*[\\S\\s]*");
    std::smatch matches;
    if (std::regex_match(line, matches, integer_pattern)) {
        std::ssub_match match = matches[1];
        return stoi(match.str());
    } else {
        std::stringstream message;
        message << "ERROR: the number on line " << line_counter << " has invalid format!" << std::endl;
        throw std::runtime_error(message.str());
    }
    return 0;
}
