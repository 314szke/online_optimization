#include "scenario.h"

#include <iomanip>
#include <iostream>


Scenario::Scenario() :
    occurrence(0),
    probability(0.0),
    terminals(0),
    solution(0),
    feasible(true)
{}

Scenario::Scenario(double prob, const std::vector<uint32_t>& terminal_list, std::vector<uint32_t> solution) :
    occurrence(1),
    probability(prob),
    terminals(terminal_list),
    solution(solution),
    feasible(true)
{}

void Scenario::checkIfStillFeasible(const std::vector<uint32_t>& current_terminals)
{
    if (! feasible || current_terminals.empty()) {
        return;
    }

    // This check is done after every terminal
    feasible = false;
    for (uint32_t idx = 0; idx < terminals.size(); idx++) {
        if (current_terminals.back() == terminals[idx]) {
            feasible = true;
        }
    }
}

bool Scenario::containsEdge(uint32_t e) const
{
    for (uint32_t idx = 0; idx < solution.size(); idx++) {
        if (solution[idx] == e) {
            return true;
        }
    }
    return false;
}

void Scenario::print(uint32_t idx, const Graph& graph) const
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << idx << ". Scenario (" << occurrence << ", " << probability << ")" << std::endl;
    Graph print_graph(graph);
    for (auto t : terminals) {
        print_graph.activate(t);
    }
    for (auto e : solution) {
        print_graph.buy(e);
    }
    print_graph.print();
}
