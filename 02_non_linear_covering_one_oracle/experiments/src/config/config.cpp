#include "config/config.h"

#include <bits/stdc++.h>
#include <math.h>


Config::Config(std::string& config_file) :
    time_horizon(0),
    max_search_iter(0),
    max_distance(0.0),
    epsilon(0.0),
    cost_degree(0.0),
    dimension(0.0),
    lambda(0.0),
    mu(0.0),
    eta(0.0),
    random_iteration(0),
    eta_step(0.0)
{
    f_in.open(config_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << config_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(time_horizon);
    readParameter(max_search_iter);
    readParameter(max_distance);
    readParameter(epsilon);
    readParameter(cost_degree);
    readParameter(random_iteration);
    readParameter(eta_step);

    f_in.close();
}

void Config::update(double new_eta)
{
    eta = new_eta;

    // O(k ln(d/eta))^(k-1)
    lambda = cost_degree * std::pow(std::log(dimension / eta),
    (cost_degree - 1.0));

    // (k-1) / (k * ln(1 + 2 * d^2 / eta))
    mu = (cost_degree - 1.0) / (cost_degree * std::log(1.0 + (2.0 * std::pow(dimension, 2.0)) / eta));
}
