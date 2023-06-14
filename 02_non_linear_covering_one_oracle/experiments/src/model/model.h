# pragma once

#include<memory>
#include<string>
#include<vector>
#include<utility>

#include "exprtk.hpp"
#include "request.hpp"
#include "types/local_types.h"


class Model {
public:
    typedef std::vector<std::vector<int64_t>> Graph_t;
    typedef std::vector<Request> RequestVec_t;

    Model();
    void parse(std::string& data_file);
    void initialize(const uint32_t n, const uint32_t m);

    uint32_t getNbVertices() const;
    uint32_t getNbEdges() const;
    uint32_t getNbRequests() const;

    const RequestVec_t& getRequests() const;
    const Graph_t& getAdjacencyMatrix() const;

    double getEdgeCost(const uint32_t edge_id, const double value);
    double getEdgeDerivative(const uint32_t edge_id, const double value);
    double getObjectiveValue(const DoubleMat_t& values);

    void addEdge(const uint32_t i, const uint32_t j, std::string f_cost, std::string f_derivative);
    void addRequest(const uint32_t i, const uint32_t j);
private:
    uint32_t nb_vertices;
    uint32_t nb_edges;
    uint32_t nb_requests;

    Graph_t A;

    RequestVec_t requests;
    DoubleVec_t costs;
    DoubleVec_t derivatives;

    double x;
    SymbolTable_t symbol_table;
    ExpressionVec_t cost_functions;
    ExpressionVec_t cost_derivatives;
    FormulaParser_t parser;
};
