#pragma once

#include <memory>

#include "types/local_types.h"


class Edge {
public:
    Edge();
    void set(uint32_t num, double coeff, double expo, double constant);

    double getCost(double value) const;
    double getDerivative(double value) const;

    int64_t id;

    double coefficient;
    double exponent;
    double constant_term;
};
