#include "edge.h"


Edge::Edge(std::string f_cost, std::string f_derivative) :
    x(0.0)
{
    symbol_table.add_variable("x", x);
    cost_function.register_symbol_table(symbol_table);
    cost_function_derivative.register_symbol_table(symbol_table);

    parser.compile(f_cost, cost_function);
    parser.compile(f_derivative, cost_function_derivative);
}

double Edge::getCost(double value)
{
    x = value;
    return cost_function.value();
}

double Edge::getDerivative(double value)
{
    x = value;
    return cost_function_derivative.value();
}
