#pragma once

#include <memory>
#include <vector>

#include "edge.h"
#include "edge_id.h"


class Graph {
public:
    typedef std::vector<std::unique_ptr<Edge>> EdgeVec_t;
    typedef std::vector<EdgeID> EdgeIDVec_t;
    typedef std::vector<EdgeVec_t> EdgeMat_t;

    Graph();
    void initialize();

    uint32_t nb_vertices;
    uint32_t nb_edges;
    EdgeMat_t A;
    EdgeIDVec_t ID;
};
