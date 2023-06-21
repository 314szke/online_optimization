# pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "graph.h"
#include "request.hpp"
#include "types/local_types.h"


class Model {
public:
    typedef std::vector<Request> RequestVec_t;

    Model(std::string& data_file);

    Graph graph;

    uint32_t nb_requests;
    RequestVec_t requests;

private:
    void parse_graph();
    void parse_requests();
    void addRequest(uint32_t i, uint32_t j);

    void readLine();
    uint32_t readInteger();

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
};
