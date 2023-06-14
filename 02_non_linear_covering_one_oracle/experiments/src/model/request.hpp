#pragma once

#include<cstdint>


class Request {
public:
    Request(const uint32_t i, const uint32_t j, const uint32_t idx) :
        source(i),
        target(j),
        id(idx)
    {}
    const uint32_t source;
    const uint32_t target;
    const uint32_t id;
};
