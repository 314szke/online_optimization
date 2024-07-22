#pragma once

#include <cstdint>
#include <vector>


class Graph {
public:
    Graph();

    void initialize(uint32_t nb_vertices);
    uint32_t IDX(uint32_t i, uint32_t j) const;
    uint32_t size() const;
    void print() const;

    void setCost(uint32_t i, uint32_t j, double cost);
    double getCost(uint32_t i, uint32_t j) const;

    void setWeight(uint32_t i, uint32_t j, double weight);
    double getWeight(uint32_t i, uint32_t j) const;

    void buy(uint32_t i, uint32_t j);
    bool isBought(uint32_t i, uint32_t j) const;

    void activate(uint32_t v);
    bool isActive(uint32_t v) const;
private:
    uint32_t n; // nb_vertices
    uint32_t m; // nb_edges

    // Edges
    std::vector<double> costs;
    std::vector<double> weights;
    std::vector<bool> is_bought;

    // Vertices
    std::vector<bool> is_active;
};
