#pragma once

#include <random>

#include "types/local_types.h"


class RandomStore {
public:
    RandomStore(uint32_t random_seed, uint32_t size);

    double getRandomNumber(uint32_t idx) const;

    std::mt19937 random_engine;
    std::uniform_real_distribution<double> uni_dist;
    DoubleVec_t random_numbers;
};
