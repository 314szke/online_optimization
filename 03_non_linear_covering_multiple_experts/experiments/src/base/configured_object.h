#pragma once

#include <fstream>
#include <regex>
#include <string>


class ConfiguredObject {
protected:
    ConfiguredObject();

    void readParameter(double& parameter);
    void readParameter(uint32_t& parameter);

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;

    std::regex config_pattern;
    std::smatch matches;
    std::ssub_match match;
};
