#pragma once

#include<string>

#include "base/configured_object.h"


class Config : private ConfiguredObject {
public:
    Config(std::string& config_file);
    void update(double new_eta);

    uint32_t time_horizon;
    uint32_t max_search_iter;
    double max_distance;
    double epsilon;
    double cost_degree;
    uint32_t dimension;
    double lambda;
    double mu;
    double eta;
    uint32_t random_iteration;
    double eta_step;
};
