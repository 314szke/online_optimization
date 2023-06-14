#include "model.h"

#include<iostream>

#include "parser.h"
#include "path_finding/CBFS.h"


Model::Model() :
    nb_vertices(0),
    nb_edges(0),
    nb_requests(0),
    x(0)
{}

void Model::parse(std::string& data_file)
{
    ModelParser parser(data_file);
    parser.parse(this);
}

void Model::initialize(const uint32_t n, const uint32_t m)
{
    nb_vertices = n;
    A.resize(nb_vertices);

    costs.resize(m);
    derivatives.resize(m);
    cost_functions.resize(m);
    cost_derivatives.resize(m);

    for (uint32_t i = 0; i < nb_vertices; i++) {
        A[i].resize(nb_vertices);
        for (uint32_t j = 0; j < nb_vertices; j++) {
            A[i][j] = -1;
        }
    }

    symbol_table.add_variable("x", x);
    for (uint32_t idx = 0; idx < m; idx++) {
        costs[idx] = 0;
        derivatives[idx] = 0;

        cost_functions[idx].register_symbol_table(symbol_table);
        cost_derivatives[idx].register_symbol_table(symbol_table);
    }
}

uint32_t Model::getNbVertices() const
{
    return nb_vertices;
}

uint32_t Model::getNbEdges() const
{
    return nb_edges;
}

uint32_t Model::getNbRequests() const
{
    return nb_requests;
}

const Model::RequestVec_t& Model::getRequests() const
{
    return requests;
}

const Model::Graph_t& Model::getAdjacencyMatrix() const
{
    return A;
}

double Model::getEdgeCost(const uint32_t edge_id, const double value)
{
    x = value;
    return cost_functions[edge_id].value();
}

double Model::getEdgeDerivative(const uint32_t edge_id, const double value)
{
    x = value;
    return cost_derivatives[edge_id].value();
}

double Model::getObjectiveValue(const DoubleMat_t& values)
{
    double cost = 0.0;
    double sum = 0.0;
    for (uint32_t e = 0; e < values[0].size(); e++) {
        sum = 0.0;
        for (uint32_t r = 0; r < values.size(); r++) {
            sum += values[r][e];
        }
        cost += getEdgeCost(e, sum);
    }
    return cost;
}

void Model::addEdge(
    const uint32_t i,
    const uint32_t j,
    std::string f_cost,
    std::string f_derivative)
{
    A[i][j] = nb_edges;
    parser.compile(f_cost, cost_functions[A[i][j]]);
    parser.compile(f_derivative, cost_derivatives[A[i][j]]);
    nb_edges++;
}

void Model::addRequest(const uint32_t i, const uint32_t j)
{
    CBFS cbfs(*this);
    BoolVec_t true_vector(nb_edges, true);
    UIntVec_t path = cbfs.getPath(i, j, true_vector);
    if (! path.empty()) {
        requests.push_back(Request(i, j, nb_requests));
        nb_requests++;
    } else {
        std::cout << "WARNING: request from " << i << " to " << j << " is not feasible!" << std::endl;
    }
}
