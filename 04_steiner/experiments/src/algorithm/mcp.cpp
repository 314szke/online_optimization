#include "mcp.h"

#include <limits>


uint32_t MCP(const Graph& graph, uint32_t terminal)
{
    uint32_t vertex;
    double cost;
    double min_cost = std::numeric_limits<double>::max();

    for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
        if (idx != terminal && graph.isActive(idx)) {
            cost = graph.getCost(idx, terminal);
            if (cost < min_cost) {
                min_cost = cost;
                vertex = idx;
            }
        }
    }

    return vertex;
}

Path MCWP(const Graph& graph, uint32_t terminal)
{
    // Initialize Dijkstra's algorithm
    std::vector<double> costs(graph.getNbVertices());
    std::vector<int> previous(graph.getNbVertices());
    std::vector<int> Q(graph.getNbVertices());
    for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
        if (idx == terminal) {
            costs[idx] = 0;
        } else {
            costs[idx] = graph.getCost(terminal, idx) * (1.0 - graph.getWeight(terminal, idx));
        }
        previous[idx] = terminal;
        Q.push_back(idx);
    }
    Q[terminal] = -1;

    uint32_t counter = 1;
    uint32_t vertex;
    double min_cost;
    double new_cost;

    // Dijkstra's algorithm
    while (counter < graph.getNbVertices()) {
        min_cost = std::numeric_limits<double>::max();
        for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
            if (Q[idx] != -1) {
                if (costs[idx] < min_cost) {
                    vertex = idx;
                    min_cost = costs[idx];
                }
            }
        }
        Q[vertex] = -1;
        counter++;

        for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
            if (Q[idx] != -1) {
                new_cost = costs[vertex] + graph.getCost(vertex, idx) * (1.0 - graph.getWeight(vertex, idx));
                if (new_cost < costs[idx]) {
                    costs[idx] = new_cost;
                    previous[idx] = vertex;
                }
            }
        }
    }

    // Find closest vertex that has already activated before
    min_cost = std::numeric_limits<double>::max();
    for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
        if (idx != terminal && graph.isActive(idx)) {
            if (costs[idx] < min_cost) {
                vertex = idx;
                min_cost = costs[idx];
            }
        }
    }

    // Build the path
    Path path;
    path.vertices.push_back(vertex);
    do {
        vertex = previous[vertex];
        path.cost += graph.getCost(path.vertices.back(), vertex);
        path.weighted_cost += graph.getCost(path.vertices.back(), vertex) * (1.0 - graph.getWeight(path.vertices.back(), vertex));
        path.vertices.push_back(vertex);
    } while (vertex != terminal);
    return path;
}
