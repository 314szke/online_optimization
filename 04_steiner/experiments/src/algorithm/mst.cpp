#include "mst.h"

#include <limits>


std::vector<uint32_t> MST(const Graph& graph, const std::vector<uint32_t>& terminals)
{
    std::vector<uint32_t> tree;
    std::vector<uint32_t> visited = {0};
    std::vector<uint32_t> remaining_terminals(terminals);

    uint32_t i,j;
    uint32_t index;
    double cost;
    double min_cost;

    while (remaining_terminals.size() > 0) {
        i = 0;
        j = 0;
        min_cost = std::numeric_limits<double>::max();

        for (uint32_t idx = 0; idx < remaining_terminals.size(); idx++) {
            for (uint32_t jdx = 0; jdx < visited.size(); jdx++) {
                cost = graph.getCost(remaining_terminals[idx], visited[jdx]);
                if (cost < min_cost) {
                    min_cost = cost;
                    i = remaining_terminals[idx];
                    j = visited[jdx];
                    index = idx;
                }
            }
        }
        tree.push_back(graph.getEdgeIndex(i,j));
        visited.push_back(i);
        remaining_terminals.erase(remaining_terminals.begin() + index);
    }

    return tree;
}
