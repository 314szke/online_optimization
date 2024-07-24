#include "greedy_solver.h"

#include <iostream>

#include "algorithm/mcp.h"


GreedySolver::GreedySolver(const Graph& graph) :
    _graph(graph)
{}

void GreedySolver::print() const
{
    std::cout << ">>> Greedy Solution <<<" << std::endl;
    _graph.print();
}

void GreedySolver::connectTerminal(uint32_t t)
{
    uint32_t vertex = MCP(_graph, t);
    _graph.buy(vertex, t);
    _graph.activate(t);
    solution.push_back(_graph.getEdgeIndex(vertex, t));
}

const std::vector<uint32_t>& GreedySolver::getSolution() const
{
    return solution;
}
