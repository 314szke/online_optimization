#include "vertex_predictor.h"

#include "algorithm/mst.h"


VertexPredictor::VertexPredictor(const Graph& graph, const std::vector<Scenario>& scenarios)
{
    double weight;
    for (uint32_t v = 1; v < graph.getNbVertices(); v++) {
        weight = 0.0;
        for (uint32_t idx = 0; idx < scenarios.size(); idx++) {
            if (scenarios[idx].containsVertex(v)) {
                weight += scenarios[idx].probability;
            }
        }
        if (weight > 0.5) {
            predicted_terminals.push_back(v);
        }
    }

    solution = MST(graph, predicted_terminals);
}

bool VertexPredictor::hasPredicted(uint32_t terminal) const
{
    for (uint32_t idx = 0; idx < predicted_terminals.size(); idx++) {
        if (predicted_terminals[idx] == terminal) {
            return true;
        }
    }
    return false;
}

const std::vector<uint32_t>& VertexPredictor::getPredictedTerminals() const
{
    return predicted_terminals;
}

const std::vector<uint32_t>& VertexPredictor::getSolution() const
{
    return solution;
}
