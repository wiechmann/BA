#include "stats.h"

#include <cmath>

double randomness(size_t unique, double entropy) {
    return 100 * entropy / std::log2(unique);
}
