#pragma once

#include <iostream>
#include <vector>


template<typename T>
void print_vector(std::string message, const std::vector<T>& vec) {
    std::cout << message << ": [";
    for (uint32_t idx = 0; idx < (vec.size() - 1); idx++) {
        std::cout << (vec[idx] + 1) << ", ";
    }
    std::cout << (vec[(vec.size() - 1)] + 1) << "]" << std::endl << std::endl;
}
