#include "scenario.h"

#include <iomanip>
#include <iostream>


Scenario::Scenario() :
    cost(0.0),
    occurrence(0),
    probability(0.0),
    terminals(0),
    solution(0),
    feasible(true)
{}

Scenario::Scenario(double obj_cost, double prob, const std::vector<uint32_t>& terminal_list, const std::vector<uint32_t>& solution) :
    cost(obj_cost),
    occurrence(1),
    probability(prob),
    terminals(terminal_list),
    solution(solution),
    feasible(true)
{}

void Scenario::checkIfStillFeasible(uint32_t current_terminal)
{
    if (! feasible || current_terminal == 0) { // 0 is root
        return;
    }

    feasible = false;
    for (uint32_t idx = 0; idx < terminals.size(); idx++) {
        if (current_terminal == terminals[idx]) {
            feasible = true;
            break;
        }
    }
}

bool Scenario::containsVertex(uint32_t v) const
{
    for (uint32_t idx = 0; idx < terminals.size(); idx++) {
        if (terminals[idx] == v) {
            return true;
        }
    }
    return false;
}

bool Scenario::containsSolution(const Graph& graph, const std::vector<uint32_t>& sub_solution)
{
    if (sub_solution.size() > solution.size()) {
        return false;
    }

    uint32_t counter = 0;
    for (uint32_t idx = 0; idx < sub_solution.size(); idx++) {
        if (graph.isBought(sub_solution[idx])) {
            counter++;
        } else {
            for (uint32_t jdx = 0; jdx < solution.size(); jdx++) {
                if (sub_solution[idx] == solution[jdx]) {
                    counter++;
                }
            }
        }
    }

    return counter == sub_solution.size();
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

void Scenario::print(uint32_t idx, const Graph& graph, uint32_t verbosity) const
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
    print_graph.print(verbosity);
}
