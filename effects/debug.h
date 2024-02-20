#pragma once

/**
 * Debugging utilities.
 */

#ifdef DEBUG

#include <iostream>

#define PLN(x) std::cerr << x << std::endl;
#define PVAR(x) std::cerr << #x << "=" << (x) << std::endl;

#endif
