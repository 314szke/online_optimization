#include "Xu_Moseley_algorithm.h"

#include <iostream>

#include "instance/path.h"


XuMoseleyAlgorithm::XuMoseleyAlgorithm(const Graph& graph, const VertexPredictor& predictor) :
    _graph(graph),
    _predictor(predictor),
    MCPF(_graph)
{}

void XuMoseleyAlgorithm::print() const
{
    std::cout << ">>> Xu-Moseley Solution <<<" << std::endl;
    _graph.print();
}

void XuMoseleyAlgorithm::connectTerminal(uint32_t t)
{
    uint32_t vertex = MinCostPathFinder::MinCostEdge(_graph, t);
    double greedy_cost = _graph.getCost(vertex, t);

    if (_predictor.hasPredicted(t)) {
        Path p = MCPF.MinCostWeightedConditionalPath(t, _predictor.getSolution());

        if (p.cost < (2 * greedy_cost)) {
            for (uint32_t idx = 1; idx < p.vertices.size(); idx++) {
                _graph.buy(p.vertices[(idx-1)], p.vertices[idx]);
                _graph.activate(p.vertices[idx]);
                solution.push_back(_graph.getEdgeIndex(p.vertices[(idx-1)], p.vertices[idx]));
            }
            return; // no need to buy the greedy edge

        } else {
            double total_cost = 0.0;
            double edge_cost;

            for (uint32_t idx = (p.vertices.size()-1); idx > 1; idx--) {
                edge_cost = _graph.getCost(p.vertices[(idx-1)], p.vertices[idx]);

                if ((2 * greedy_cost) > (total_cost + edge_cost)) {
                    _graph.buy(p.vertices[(idx-1)], p.vertices[idx]);
                    _graph.activate(p.vertices[idx-1]);
                    solution.push_back(_graph.getEdgeIndex(p.vertices[(idx-1)], p.vertices[idx]));
                    total_cost += edge_cost;

                } else {
                    break;
                }
            }
        }
    }

    _graph.buy(vertex, t);
    _graph.activate(t);
    solution.push_back(_graph.getEdgeIndex(vertex, t));
}

const std::vector<uint32_t>& XuMoseleyAlgorithm::getSolution() const
{
    return solution;
}
