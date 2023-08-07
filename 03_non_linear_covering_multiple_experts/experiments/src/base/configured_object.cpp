#include "configured_object.h"


ConfiguredObject::ConfiguredObject() :
    line_counter(0),
    config_pattern("[a-zA-Z_]+\\s+=\\s+([0-9\\.]*)\\s+[\\S\\s]*")
{}

void ConfiguredObject::readParameter(double& parameter)
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

void ConfiguredObject::readParameter(uint32_t& parameter)
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
