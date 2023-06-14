#pragma once

#include<iostream>
#include<string>
#include<vector>


template<typename T>
void print_vector(std::string message, const std::vector<T>& vec) {
    std::cout << message << ": [";
    for (uint32_t idx = 0; idx < (vec.size() - 1); idx++) {
        /*
        if (vec[idx] > 0.001) {
            //std::cout << "(" << idx << ": " << vec[idx] << "), ";
        }
        */
        std::cout << vec[idx] << ", ";
    }
    std::cout << vec[(vec.size() - 1)] << "]" << std::endl;
}

template<typename T>
void print_solution(std::string message, double value, const std::vector<std::vector<T>>& vec) {
    std::cout << message << " = " << value << std::endl;
    for (uint32_t r = 0; r < vec.size(); r++) {
        print_vector(std::to_string((r + 1)), vec[r]);
    }
}
