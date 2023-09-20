#pragma once

#include <string>

#include "base/configured_object.h"


class Config : private ConfiguredObject {
public:
    Config(const std::string& config_file);
    void update(double new_eta);

    uint32_t time_horizon;
    uint32_t max_search_iter;
    double max_distance;
    uint32_t gurobi_verbosity;
    double epsilon;
    double cost_degree;
    uint32_t dimension;
    double lambda;
    double mu;
    double eta;
    uint32_t random_iteration;
    double DT;
    double eta_step;
    uint32_t nb_oracles;
    uint32_t nb_oracle_search;
    uint32_t random_seed;
    uint32_t random_store_size;
    uint32_t use_threads;
};
