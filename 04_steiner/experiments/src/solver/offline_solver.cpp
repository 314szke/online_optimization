#include "offline_solver.h"

#include <iostream>

#include "algorithm/mst.h"


OfflineSolver::OfflineSolver(const Graph& graph, const std::vector<uint32_t>& terminals) :
    _graph(graph),
    _terminals(terminals)
{
    solution = MST(_graph, terminals);
    for (auto edge : solution) {
        _graph.buy(edge);
    }
    for (auto t : terminals) {
        _graph.activate(t);
    }
}

void OfflineSolver::print() const
{
    std::cout << ">>> Offline Solution <<<" << std::endl;
    std::cout << "Terminals: [";
    for (uint32_t idx = 0; idx < (_terminals.size() - 1); idx++) {
        std::cout << (_terminals[idx] + 1) << ", ";
    }
    std::cout << (_terminals[(_terminals.size() - 1)] + 1) << "]" << std::endl;

    _graph.print();
}

const std::vector<uint32_t>& OfflineSolver::getSolution() const
{
    return solution;
}
