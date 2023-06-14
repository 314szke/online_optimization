#include "config/config.h"


Config::Config(const std::string& config_file) :
    time_horizon(0),
    max_search_iter(0),
    max_distance(0.0),
    epsilon(0.0)
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
    readParameter(gurobi_verbosity);
    readParameter(epsilon);

    f_in.close();
}
