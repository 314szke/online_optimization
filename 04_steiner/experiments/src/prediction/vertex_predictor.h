#pragma once


#include <cstdint>
#include <vector>

#include "instance/graph.h"
#include "instance/scenario.h"


class VertexPredictor {
public:
    VertexPredictor(const Graph& graph, const std::vector<Scenario>& scenarios);

    bool hasPredicted(uint32_t terminal) const;
    const std::vector<uint32_t>& getPredictedTerminals() const;
    const std::vector<uint32_t>& getSolution() const;
private:
    std::vector<uint32_t> predicted_terminals;
    std::vector<uint32_t> solution;
};
