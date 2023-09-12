#pragma once

#include <memory>

#include "exprtk.hpp"
#include "types/local_types.h"


class Edge {
public:
    Edge();
    void set(uint32_t num, std::string f_cost, std::string f_derivative);

    double getCost(double value);
    double getDerivative(double value);

    int64_t id;

private:
    typedef exprtk::symbol_table<double> SymbolTable_t;
    typedef exprtk::expression<double> Expression_t;
    typedef exprtk::parser<double> FormulaParser_t;

    double x;
    Expression_t cost_function;
    Expression_t cost_function_derivative;

    SymbolTable_t symbol_table;
    std::unique_ptr<FormulaParser_t> parser;
};
