#pragma once

#include <vector>

#include "instance/graph.h"


std::vector<uint32_t> MST(const Graph& graph, const std::vector<uint32_t>& terminals);
