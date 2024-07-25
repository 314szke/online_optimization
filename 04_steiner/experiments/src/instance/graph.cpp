#include "graph.h"

#include <algorithm>
#include <iomanip>
#include <iostream>


Graph::Graph() :
    n(0),
    m(0),
    costs(0),
    weights(0),
    is_bought(0),
    is_active(0)
{}

Graph::Graph(const Graph& graph) :
    n(graph.n),
    m(graph.m),
    costs(graph.costs),
    weights(graph.weights),
    is_bought(graph.is_bought),
    is_active(graph.is_active)
{}

void Graph::initialize(uint32_t nb_vertices)
{
    n = nb_vertices;
    m = (n*(n-1))/2;
    costs.resize(m, 1.0);
    weights.resize(m, 1.0);
    is_bought.resize(m, false);
    is_active.resize(nb_vertices, false);
}

uint32_t Graph::getNbVertices() const
{
    return n;
}

uint32_t Graph::getNbEdges() const
{
    return m;
}

double Graph::getSteinerCost() const
{
    double value = 0;
    for (uint32_t edge = 0; edge < m; edge++) {
        if (is_bought[edge]) {
            value += costs[edge];
        }
    }
    return value;
}

double Graph::getSteinerCost(const std::vector<uint32_t>& solution) const
{
    // Make sure there are no duplicates
    std::vector<uint32_t> edges = solution;
    std::sort(edges.begin(), edges.end());
    auto last = std::unique(edges.begin(), edges.end());
    edges.erase(last, edges.end());

    double value = 0;
    for (auto edge : edges) {
        if (edge > m) {
            throw std::runtime_error("ERROR: Edge index out of range!\n");
        }
        if (is_bought[edge]) {
            value += costs[edge];
        }
    }
    return value;
}

void Graph::print() const
{
    std::cout << std::fixed << std::setprecision(1);
    for (uint32_t i = 0; i < n; i++) {
        std::cout << "\t" << (i+1) << "\t";
    }
    std::cout << std::endl;

    for (uint32_t i = 0; i < n; i++) {
        if (is_active[i]) {
            std::cout << "\033[1;31m"; // red on
            std::cout << (i+1) << "\t";
            std::cout << "\033[1;0m"; // red off
        } else {
            std::cout << (i+1) << "\t";
        }
        for (uint32_t j = 0; j < (i+1); j++) {
            std::cout << "\033[1;30m"; // black on
            std::cout << "(0.0, 0.0)\t";
            std::cout << "\033[1;0m"; // black off
        }
        for (uint32_t j = (i+1); j < n; j++) {
            if (isBought(i,j)) {
                std::cout << "\033[1;31m"; // red on
                std::cout << "\e[1m"; // bold on
                std::cout << "(" << getCost(i,j) << ", " << getWeight(i,j) << ")\t";
                std::cout << "\e[0m"; // bold off
                std::cout << "\033[1;0m"; // red off
            } else {
                std::cout << "\e[1m"; // bold on
                std::cout << "(" << getCost(i,j) << ", " << getWeight(i,j) << ")\t";
                std::cout << "\e[0m"; // bold off
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Steiner tree cost = ";
    std::cout << "\033[1;33m"; // yellow on
    std::cout << "\e[1m"; // bold on
    std::cout << getSteinerCost() << std::endl << std::endl;
    std::cout << "\e[0m"; // bold off
    std::cout << "\033[1;0m"; // yellow off
}

void Graph::setCost(uint32_t i, uint32_t j, double cost)
{
    costs[getEdgeIndex(i,j)] = cost;
}

double Graph::getCost(uint32_t i, uint32_t j) const
{
    return costs[getEdgeIndex(i,j)];
}

double Graph::getCurrentCost(uint32_t e) const
{
    if (e > m) {
        throw std::runtime_error("ERROR: Edge index out of range!\n");
    }
    if (is_bought[e]) {
        return 0;
    }
    return costs[e];
}

double Graph::getCurrentCost(uint32_t i, uint32_t j) const
{
    return getCurrentCost(getEdgeIndex(i,j));
}

void Graph::setWeight(uint32_t e, double weight)
{
    if (e > m) {
        throw std::runtime_error("ERROR: Edge index out of range!\n");
    }

    weights[e] = weight;
}

void Graph::setWeight(uint32_t i, uint32_t j, double weight)
{
    weights[getEdgeIndex(i,j)] = weight;
}

double Graph::getWeight(uint32_t e) const
{
    if (e > m) {
        throw std::runtime_error("ERROR: Edge index out of range!\n");
    }
    return weights[e];
}

double Graph::getWeight(uint32_t i, uint32_t j) const
{
    return weights[getEdgeIndex(i,j)];
}

void Graph::buy(uint32_t e)
{
    if (e > m) {
        throw std::runtime_error("ERROR: Edge index out of range!\n");
    }
    is_bought[e] = true;
}

void Graph::buy(uint32_t i, uint32_t j)
{
    is_bought[getEdgeIndex(i,j)] = true;
}

bool Graph::isBought(uint32_t e) const
{
    if (e > m) {
        throw std::runtime_error("ERROR: Edge index out of range!\n");
    }
    return is_bought[e];
}

bool Graph::isBought(uint32_t i, uint32_t j) const
{
    return is_bought[getEdgeIndex(i,j)];
}

void Graph::activate(uint32_t v)
{
    is_active[v] = true;
}

bool Graph::isActive(uint32_t v) const
{
    return is_active[v];
}

uint32_t Graph::getEdgeIndex(uint32_t i, uint32_t j) const
{
    if (i == j) {
        throw std::runtime_error("ERROR: No self loops!\n");
    }

    if (i >= n || j>= n) {
        throw std::runtime_error("ERROR: Index out of bound!\n");
    }

    if (i > j) {
        uint32_t tmp = i;
        i = j;
        j = tmp;
    }

    return (m - ((n-i-1)*(n-i))/2 + (j-i-1));
}
