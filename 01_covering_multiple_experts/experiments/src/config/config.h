#pragma once

#include <string>

#include "base/configured_object.h"


class Config : private ConfiguredObject {
public:
    Config(const std::string& config_file);

    uint32_t time_horizon;
    uint32_t max_search_iter;
    double max_distance;
    uint32_t gurobi_verbosity;
    double epsilon;
    uint32_t L;
    bool is_convex;
};
