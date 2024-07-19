#include "config.h"


Config::Config(const std::string& config_file) :
    nb_vertices(0),
    min_terminals(0),
    max_terminals(0),
    nb_scenarios(0),
    dimension_size(0),
    random_seed(0),
    line_counter(0),
    config_pattern("[a-zA-Z_]+\\s+=\\s+([0-9\\.]*)\\s*[\\S\\s]*")
{
    f_in.open(config_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << config_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(nb_vertices);
    readParameter(min_terminals);
    readParameter(max_terminals);
    readParameter(nb_scenarios);
    readParameter(dimension_size);
    readParameter(random_seed);

    f_in.close();
}

void Config::readParameter(uint32_t& parameter)
{
    line.resize(0);
    std::getline(f_in, line);
    line_counter++;

    if (line.empty()) {
        std::stringstream message;
        message << "ERROR: line " << line_counter << " should not be empty!" << std::endl;
        throw std::runtime_error(message.str());
    }

    if (std::regex_match(line, matches, config_pattern)) {
        match = matches[1];
        std::stringstream ss;
        ss << match.str();
        ss >> parameter;
    } else {
        std::stringstream message;
        message << "ERROR: the parameter on line " << line_counter << " has invalid format!" << std::endl;
        throw std::runtime_error(message.str());
    }
}