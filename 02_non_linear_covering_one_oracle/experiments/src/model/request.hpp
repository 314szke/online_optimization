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

    const uint32_t id;
    const uint32_t source;
    const uint32_t target;
};
