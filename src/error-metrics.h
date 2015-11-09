#pragma once
#include "opencv2/core/core.hpp"
#include <math.h>

using namespace cv;

#define EVAL_BAD_THRESH 1

/*
  rms of non-occluded regions
  rms of textured regions

  accuracy of occluded vs. non-occluded classification
*/
class ErrorMetrics {

public:
	static double get_rms_error_all (const Mat gold_disparity, const Mat guess_disparity) ;
	static double get_bad_matching_all (const Mat gold_disparity, const Mat guess_disparity) ;

};