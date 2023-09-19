#include "random_store.h"


RandomStore::RandomStore(uint32_t random_seed, uint32_t size) :
    random_engine(random_seed),
    uni_dist(0.0, 1.0),
    random_numbers(size)
{
    for (uint32_t idx = 0; idx < random_numbers.size(); idx++) {
        random_numbers[idx] = uni_dist(random_engine);
    }
}

double RandomStore::getRandomNumber(uint32_t idx) const
{
    return random_numbers[idx];
}
