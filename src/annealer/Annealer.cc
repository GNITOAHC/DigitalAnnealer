#include <functional>
#include <random>

#include "Annealer.h"

Annealer::Annealer(const int r) : myrank(r) {}

// Randomly execute the given function with probability rand
bool Annealer::randomExec(const double rand, const std::function<void()> func) {
    // Random number generator
    std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    if (dis(generator) < rand) {
        func();
        return true;
    }

    return false;
}
