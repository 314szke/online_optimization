#pragma once


class EdgeID {
public:
    EdgeID(uint32_t v1, uint32_t v2) :
        i(v1),
        j(v2)
    {}

    uint32_t i;
    uint32_t j;
};
