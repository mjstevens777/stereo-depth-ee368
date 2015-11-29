#pragma once
#include "stereo-pair.h"

class DisparityAlgorithm {
public:
  DisparityAlgorithm& compute(StereoPair &pair);
};

// template<typename Algorithm>
// static void calculate_disparity(StereoPair &pair) {
//   Algorithm alg;
//   alg.compute(pair);
// }