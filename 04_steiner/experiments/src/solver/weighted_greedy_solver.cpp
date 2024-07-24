#include "weighted_greedy_solver.h"

#include <iostream>

#include "algorithm/mcp.h"
#include "instance/path.h"


WeightedGreedySolver::WeightedGreedySolver(Graph& graph) :
    _graph(graph)
{}

void WeightedGreedySolver::print() const
{
    std::cout << ">>> Weighted Greedy Solution <<<" << std::endl;
    _graph.print();
}

void WeightedGreedySolver::connectTerminal(uint32_t t)
{
    Path p = MCWP(_graph, t);
    for (uint32_t idx = 1; idx < p.vertices.size(); idx++) {
        _graph.buy(p.vertices[(idx-1)], p.vertices[idx]);
        _graph.activate(p.vertices[idx]);
        solution.push_back(_graph.getEdgeIndex(p.vertices[(idx-1)], p.vertices[idx]));
    }
}

const std::vector<uint32_t>& WeightedGreedySolver::getSolution() const
{
    return solution;
}
