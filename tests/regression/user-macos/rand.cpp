#include <rand.h>

std::uniform_real_distribution<float> RNG::floatU;
std::random_device RNG::device;
std::mt19937 RNG::engine(device());
