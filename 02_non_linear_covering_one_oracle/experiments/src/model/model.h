# pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "request.hpp"
#include "types/local_types.h"


class Model {
public:
    typedef std::vector<Request> RequestVec_t;

    Model(std::string& data_file);

    double getObjectiveValue(const DoubleMat_t& values);

    Graph graph;

    uint32_t nb_requests;
    RequestVec_t requests;

private:
    void parse_graph();
    void parse_requests();
    void addRequest(const uint32_t i, const uint32_t j);

    void readLine();
    uint32_t readInteger();

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
};
