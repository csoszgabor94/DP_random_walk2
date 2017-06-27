#include <random>

#include "Random.h"

random_engine& get_random_engine(random_engine::result_type seed) {
	static random_engine engine{seed};
	return engine;
}
