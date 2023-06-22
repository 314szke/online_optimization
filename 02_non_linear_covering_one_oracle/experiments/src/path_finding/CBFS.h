#pragma once

#include <vector>
#include <cstdint>

typedef std::vector<int64_t> IntVec_t;
typedef std::vector<uint32_t> UIntVec_t;
typedef std::vector<bool> BoolVec_t;
typedef std::vector<std::vector<int64_t>> adjacency_t;

// Conditional Breadth-First Search
class CBFS {
public:
    CBFS(const adjacency_t& A);
    UIntVec_t getPath(const uint32_t s, const uint32_t t, const BoolVec_t& marker);
private:
    bool pathExists(const uint32_t s, const uint32_t t, const BoolVec_t& marker);

    const adjacency_t _graph;
    BoolVec_t visited;
    IntVec_t prev;
};
