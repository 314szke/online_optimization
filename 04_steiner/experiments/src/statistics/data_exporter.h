#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "instance/instance.h"


class DataExporter {
public:
    DataExporter(
        const std::string& test_name,
        const Instance& instance,
        const std::vector<uint32_t>& offline_solution,
        const std::vector<uint32_t>& xm_solution,
        const std::vector<uint32_t>& greedy_solution,
        const std::vector<uint32_t>& solution);

    void exportToFile();
private:
    const std::string& _test_name;
    const Instance& _instance;
    const std::vector<uint32_t>& _offline_solution;
    const std::vector<uint32_t>& _xm_solution;
    const std::vector<uint32_t>& _greedy_solution;
    const std::vector<uint32_t>& _solution;
};
