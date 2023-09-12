#pragma once

#include <vector>

#include "edge.h"
#include "edge_id.hpp"


class Graph {
public:
    typedef std::vector<Edge> EdgeVec_t;
    typedef std::vector<EdgeID> EdgeIDVec_t;
    typedef std::vector<EdgeVec_t> EdgeMat_t;
    typedef std::vector<uint32_t> Path_t;

    Graph();

    void initialize(uint32_t n, uint32_t m);
    void findAllPaths();
    Path_t getPath(uint32_t s, uint32_t t);

    uint32_t nb_vertices;
    uint32_t nb_edges;
    EdgeMat_t A;
    EdgeIDVec_t ID;

private:

    UIntMat_t distances;
    IntMat_t connections;
};
