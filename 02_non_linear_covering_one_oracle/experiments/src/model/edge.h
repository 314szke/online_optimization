#pragma once

#include <memory>

#include "types/local_types.h"


class Edge {
public:
    Edge();
    void set(uint32_t num, double coeff, double expo, double constant);

    double getCost(double value);
    double getDerivative(double value);

    int64_t id;

private:
    double coefficient;
    double exponent;
    double constant_term;
};
