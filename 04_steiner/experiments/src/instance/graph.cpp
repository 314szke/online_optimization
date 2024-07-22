#include "graph.h"

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

void Graph::initialize(uint32_t nb_vertices)
{
    n = nb_vertices;
    m = (n*(n-1))/2;
    costs.resize(m, 1.0);
    weights.resize(m, 1.0);
    is_bought.resize(m, false);
    is_active.resize(nb_vertices, false);
}

uint32_t Graph::IDX(uint32_t i, uint32_t j) const
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

uint32_t Graph::size() const
{
    return n;
}

void Graph::print() const
{
    std::cout << std::fixed << std::setprecision(1);
    for (uint32_t i = 0; i < n; i++) {
        std::cout << "    " << (i+1) << "          ";
    }
    std::cout << std::endl;

    for (uint32_t i = 0; i < n; i++) {
        std::cout << (i+1) << "  ";
        for (uint32_t j = 0; j < (i+1); j++) {
            std::cout << "(0.0, 0.0, 0)  ";
        }
        for (uint32_t j = (i+1); j < n; j++) {
            std::cout << "\e[1m"; // bold on
            std::cout << "(" << getCost(i,j) << ", " << getWeight(i,j) << ", " << isBought(i,j) << ")  ";
            std::cout << "\e[0m"; // bold off
        }
        std::cout << std::endl;
    }
}

void Graph::setCost(uint32_t i, uint32_t j, double cost)
{
    costs[IDX(i,j)] = cost;
}

double Graph::getCost(uint32_t i, uint32_t j) const
{
    return costs[IDX(i,j)];
}

void Graph::setWeight(uint32_t i, uint32_t j, double weight)
{
    weights[IDX(i,j)] = weight;
}

double Graph::getWeight(uint32_t i, uint32_t j) const
{
    return weights[IDX(i,j)];
}

void Graph::buy(uint32_t i, uint32_t j)
{
    is_bought[IDX(i,j)] = true;
}

bool Graph::isBought(uint32_t i, uint32_t j) const
{
    return is_bought[IDX(i,j)];
}

void Graph::activate(uint32_t v)
{
    is_active[v] = true;
}

bool Graph::isActive(uint32_t v) const
{
    return is_active[v];
}
