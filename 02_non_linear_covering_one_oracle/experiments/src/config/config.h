#pragma once

#include<fstream>
#include<regex>
#include<string>


class Config {
public:
    Config();
    void parse(std::string& config_file);
    void calculateLambda();
    void calculateMu();

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
private:
    template<typename T>
    void readParameter(T& parameter);

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
    std::regex config_pattern;
    std::smatch matches;
    std::ssub_match match;
};
