#include "edge.h"

#include <cmath>


Edge::Edge() :
    id(-1),
    coefficient(1.0),
    exponent(0.0)
{}

void Edge::set(uint32_t num, double coeff, double expo, double constant)
{
    id = num;
    coefficient = coeff;
    exponent = expo;
    constant_term = constant;
}

double Edge::getCost(double value)
{
    return (coefficient * std::pow(value, exponent)) + constant_term;
}

double Edge::getDerivative(double value)
{
    return (coefficient * exponent) * std::pow(value, (exponent-1));
}
