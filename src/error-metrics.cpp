#include "error-metrics.h"
#include <math.h>
#include <iostream>

using namespace cv;

double ErrorMetrics::get_rms_error_all (const Mat gold_disparity, const Mat guess_disparity) {
	int num_pixel = gold_disparity.rows * gold_disparity.cols;

	// sqrt(1/num_pixel*sum(diff^2))
	Mat diff = gold_disparity - guess_disparity;
	double score = 1/sqrt(num_pixel)* norm(diff,NORM_L2);

	return score;
}

double ErrorMetrics::get_bad_matching_all (const Mat gold_disparity, const Mat guess_disparity)  {
	int num_pixel = gold_disparity.rows * gold_disparity.cols;

	// sqrt(1/num_pixel*sum( abs(diff) > thresh))
	Mat abs_diff = abs(gold_disparity - guess_disparity);
	Mat is_bad = (abs_diff > EVAL_BAD_THRESH) / 255;
	double num_bad_pixel = sum(is_bad)[0];
	double score = num_bad_pixel/num_pixel;

	return score;
}