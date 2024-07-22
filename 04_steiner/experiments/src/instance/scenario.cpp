#include "scenario.h"


Scenario::Scenario() :
    occurrence(0),
    probability(0.0),
    terminals(0),
    solution(0)
{}

Scenario::Scenario(double prob, const std::vector<uint32_t>& terminal_list, std::vector<uint32_t> solution) :
    occurrence(1),
    probability(prob),
    terminals(terminal_list),
    solution(solution)
{}
