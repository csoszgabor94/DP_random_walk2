#ifndef RANDOM_H
#define RANDOM_H

#include <random>

using random_engine = std::mt19937;

random_engine& get_random_engine(
    random_engine::result_type seed = random_engine::default_seed);

#endif  // RANDOM_H
