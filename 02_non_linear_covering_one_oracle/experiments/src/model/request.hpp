#pragma once

#include <cstdint>


class Request {
public:
    Request(const uint32_t idx, const uint32_t i, const uint32_t j) :
        id(idx),
        source(i),
        target(j)
    {}

    Request(const Request& other) :
        id(other.id),
        source(other.source),
        target(other.target)
    {}

    friend bool operator <(const Request& x, const Request& y) {
        return x.id < y.id;
    }

    uint32_t id;
    uint32_t source;
    uint32_t target;
};
