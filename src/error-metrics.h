#pragma once
#include "opencv2/core/core.hpp"
#include <math.h>
#include <tuple>

using namespace cv;

#define EVAL_BAD_THRESH 5

/*
  rms of non-occluded regions
  rms of textured regions

  accuracy of occluded vs. non-occluded classification
*/
class ErrorMetrics {

public:
	static double get_rms_error_all (const Mat gold_disparity, const Mat guess_disparity) ;
	static double get_bad_matching_all (const Mat gold_disparity, const Mat guess_disparity) ;

  static cv::Mat get_unoccluded (const Mat gold_disparity, const Mat guess_disparity) ;
  static std::tuple<cv::Mat, cv::Mat, int> get_unoccluded_diff (const Mat gold_disparity, const Mat guess_disparity) ;

  static double get_rms_error_unoccluded (const Mat gold_disparity, const Mat guess_disparity) ;
  static double get_correlation_unoccluded (const Mat gold_disparity, const Mat guess_disparity) ;
  static double get_bias_unoccluded (const Mat gold_disparity, const Mat guess_disparity) ;
  static double get_r_squared_unoccluded (const Mat gold_disparity, const Mat guess_disparity) ;

};