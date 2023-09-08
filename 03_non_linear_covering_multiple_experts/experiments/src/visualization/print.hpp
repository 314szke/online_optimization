#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>


template<typename T>
void print_vector(std::string message, const std::vector<T>& vec) {
    std::cout << message << ": [";
    for (uint32_t idx = 0; idx < (vec.size() - 1); idx++) {
        std::cout << vec[idx] << ", ";
    }
    std::cout << vec[(vec.size() - 1)] << "]" << std::endl;
}

template<typename T>
void print_matrix(std::string message, const std::vector<std::vector<T>>& vec) {
    std::cout << message << std::endl;
    for (uint32_t idx = 0; idx < vec.size(); idx++) {
        std::stringstream new_message;
        new_message << (idx + 1);
        print_vector(new_message.str(), vec[idx]);
    }
}

template<typename T>
void print_solution(std::string message, double value, const std::vector<std::vector<T>>& vec) {
    std::stringstream new_message;
    new_message << message << " = " << value;
    print_matrix(new_message.str(), vec);
}

template<typename T>
void print_solution(std::string message, double value, const std::vector<T>& vec) {
    std::stringstream new_message;
    new_message << message << " = " << value << std::endl << message;
    if (vec.size() > 25) {
        std::cout << new_message.str() << " [too long to print]" << std::endl;
    } else {
        print_vector(new_message.str(), vec);
    }
}
