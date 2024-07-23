#pragma once

#include <vector>

#include "instance/graph.h"
#include "instance/path.h"


uint32_t MCP(const Graph& graph, uint32_t terminal);

Path MCWP(const Graph& graph, uint32_t terminal);
