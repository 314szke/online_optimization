#include "scenario.h"

#include <iomanip>
#include <iostream>


Scenario::Scenario() :
    occurrence(0),
    probability(0.0),
    terminals(0),
    solution(0)
{}

Scenario::Scenario(double prob, const std::vector<uint32_t>& terminal_list, std::vector<uint32_t> solution) :
    occurrence(1),
    probability(prob),
    terminals(terminal_list),
    solution(solution)
{}

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
