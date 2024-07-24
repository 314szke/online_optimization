#include "mcp.h"

#include <limits>


MinCostPathFinder::MinCostPathFinder(const Graph& graph) :
    _graph(graph),
    costs(_graph.getNbVertices()),
    arrived_from(_graph.getNbVertices()),
    Q(_graph.getNbVertices())
{}

uint32_t MinCostPathFinder::MinCostEdge(const Graph& graph, uint32_t terminal)
{
    uint32_t vertex;
    double cost;
    double min_cost = std::numeric_limits<double>::max();

    for (uint32_t idx = 0; idx < graph.getNbVertices(); idx++) {
        if (idx != terminal && graph.isActive(idx)) {
            cost = graph.getCurrentCost(idx, terminal);
            if (cost < min_cost) {
                min_cost = cost;
                vertex = idx;
            }
        }
    }

    return vertex;
}

Path MinCostPathFinder::MinCostWeightedPath(uint32_t terminal)
{
    std::vector<uint32_t> no_condition;
    return MinCostWeightedConditionalPath(terminal, no_condition);
}

Path MinCostPathFinder::MinCostWeightedConditionalPath(uint32_t terminal, const std::vector<uint32_t>& condition)
{
    initialize(terminal, condition);

    // Dijkstra's algorithm
    uint32_t vertex;
    uint32_t counter = 1;
    while (counter < _graph.getNbVertices()) {
        vertex = getClosestUnvisitedVertex();
        Q[vertex] = -1; // = visited
        counter++;
        updateCosts(vertex, condition);
    }

    return buildPath(terminal, getClosestActiveVertex(terminal));
}



void MinCostPathFinder::initialize(uint32_t terminal, const std::vector<uint32_t>& condition)
{
    uint32_t edge;
    for (uint32_t idx = 0; idx < _graph.getNbVertices(); idx++) {
        if (idx == terminal) {
            costs[idx] = 0;
        } else {
            edge = _graph.getEdgeIndex(terminal, idx);
            if (edgeSatisfiesCondition(edge, condition)) {
                costs[idx] = _graph.getCurrentCost(edge) * (1.0 - _graph.getWeight(edge));
            } else {
                costs[idx] = std::numeric_limits<double>::max();
            }
        }
        arrived_from[idx] = terminal;
        Q[idx] = idx;
    }
    Q[terminal] = -1; // = visited
}

bool MinCostPathFinder::edgeSatisfiesCondition(uint32_t edge, const std::vector<uint32_t>& condition)
{
    if (condition.empty()) {
        return true;
    }

    for (uint32_t idx = 0; idx < condition.size(); idx++) {
        if (condition[idx] == edge) {
            return true;
        }
    }
    return false;
}

uint32_t MinCostPathFinder::getClosestUnvisitedVertex()
{
    uint32_t vertex;
    double min_cost = std::numeric_limits<double>::max();
    for (uint32_t idx = 0; idx < _graph.getNbVertices(); idx++) {
        if (Q[idx] != -1) {
            if (costs[idx] < min_cost) {
                vertex = idx;
                min_cost = costs[idx];
            }
        }
    }
    return vertex;
}

void MinCostPathFinder::updateCosts(uint32_t vertex, const std::vector<uint32_t>& condition)
{
    uint32_t edge;
    double new_cost;

    for (uint32_t idx = 0; idx < _graph.getNbVertices(); idx++) {
        if (Q[idx] != -1) {

            edge = _graph.getEdgeIndex(vertex, idx);
            if (edgeSatisfiesCondition(edge, condition)) {
                new_cost = costs[vertex] + _graph.getCurrentCost(edge) * (1.0 - _graph.getWeight(edge));

                if (new_cost < costs[idx]) {
                    costs[idx] = new_cost;
                    arrived_from[idx] = vertex;
                }
            }
        }
    }
}

uint32_t MinCostPathFinder::getClosestActiveVertex(uint32_t terminal)
{
    uint32_t vertex;
    double min_cost = std::numeric_limits<double>::max();
    for (uint32_t idx = 0; idx < _graph.getNbVertices(); idx++) {
        if (idx != terminal && _graph.isActive(idx)) {
            if (costs[idx] < min_cost) {
                vertex = idx;
                min_cost = costs[idx];
            }
        }
    }
    return vertex;
}

Path MinCostPathFinder::buildPath(uint32_t terminal, uint32_t vertex)
{
    Path path;
    uint32_t edge;
    path.vertices.push_back(vertex);
    do {
        vertex = arrived_from[vertex];
        edge = _graph.getEdgeIndex(path.vertices.back(), vertex);

        path.cost += _graph.getCurrentCost(edge);
        path.weighted_cost += _graph.getCurrentCost(edge) * (1.0 - _graph.getWeight(edge));

        path.vertices.push_back(vertex);
        if (!_graph.isBought(edge)) {
            path.edges.push_back(edge);
        }
    } while (vertex != terminal);

    return path;
}
