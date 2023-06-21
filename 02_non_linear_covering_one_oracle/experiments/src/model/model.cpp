#include "model.h"

#include <iostream>
#include <regex>

#include "edge.h"
#include "edge_id.hpp"


Model::Model(std::string& data_file) :
    nb_requests(0),
    f_in(data_file),
    line_counter(0)
{
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << data_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    uint32_t nb_vertices = readInteger();
    uint32_t nb_edges = readInteger();
    graph.initialize(nb_vertices, nb_edges);

    parse_graph();
    graph.findAllPaths();
    parse_requests();

    f_in.close();
}

void Model::parse_graph()
{
    const std::regex edge_parameters("\\s*([0-9]+)\\s*-\\s*([0-9]+)\\s*#\\s*([x0-9\\.\\*\\^\\-\\+\\/\\s]+)\\s*#\\s*([x0-9\\.\\*\\^\\-\\+\\/\\s]+)\\s*[\\S\\s]*");

    std::smatch matched_parameters;
    uint32_t i, j;
    std::ssub_match match_1;
    std::ssub_match match_2;

    for (uint32_t idx = 0; idx < graph.nb_edges; idx++) {
        readLine();
        if (std::regex_match(line, matched_parameters, edge_parameters)) {
            match_1 = matched_parameters[1];
            match_2 = matched_parameters[2];

            i = stoi(match_1.str());
            j = stoi(match_2.str());

            match_1 = matched_parameters[3];
            match_2 = matched_parameters[4];

            graph.A[i][j].reset(new Edge(idx, match_1.str(), match_2.str()));
            graph.ID.push_back(EdgeID(i, j));
        } else {
            std::stringstream message;
            message << "ERROR: edge on line " << line_counter << " has invalid format!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }
}

void Model::parse_requests()
{
    uint32_t original_nb_requests = readInteger();

    const std::regex request_parameters("\\s*([0-9]+)\\s*-\\s*([0-9]+)\\s*[\\S\\s]*");

    std::smatch matched_parameters;
    uint32_t i, j;
    std::ssub_match match_1;
    std::ssub_match match_2;

    for (uint32_t idx = 0; idx < original_nb_requests; idx++) {
        readLine();
        if (std::regex_match(line, matched_parameters, request_parameters)) {
            match_1 = matched_parameters[1];
            match_2 = matched_parameters[2];

            i = stoi(match_1.str());
            j = stoi(match_2.str());

            addRequest(i, j);
        } else {
            std::stringstream message;
            message << "ERROR: request on line " << line_counter << " has invalid format!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }
}

void Model::addRequest(uint32_t i, uint32_t j)
{
    Graph::Path_t path = graph.getPath(i, j);
    if (! path.empty()) {
        requests.push_back(Request(nb_requests, i, j));
        nb_requests++;
    } else {
        std::cout << "WARNING: request from " << i << " to " << j << " is not feasible!" << std::endl;
    }
}

void Model::readLine()
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

uint32_t Model::readInteger()
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
