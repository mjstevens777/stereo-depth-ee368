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

cv::Mat ErrorMetrics::get_unoccluded (const Mat gold_disparity, const Mat guess_disparity) {
	cv::Mat gold_unoccluded = (gold_disparity != 0);
	cv::Mat guess_unoccluded = (guess_disparity != 0);

	return (gold_unoccluded) & (guess_unoccluded);
}

// returns (residual, unoccluded, num unoccluded)
std::tuple<Mat, Mat, int> ErrorMetrics::get_unoccluded_diff (const Mat gold_disparity, const Mat guess_disparity) {
	Mat unoccluded_mask = get_unoccluded(gold_disparity, guess_disparity);
	int num_pixel = countNonZero(unoccluded_mask);

	Mat guess_disparity_f, gold_disparity_f;

	guess_disparity.convertTo(guess_disparity_f, CV_32FC1);
	gold_disparity.convertTo(gold_disparity_f, CV_32FC1);

	Mat diff = guess_disparity_f - gold_disparity_f;
	diff.setTo(0, unoccluded_mask == 0);
	return std::make_tuple(diff, unoccluded_mask, num_pixel);
}

double ErrorMetrics::get_rms_error_unoccluded (const Mat gold_disparity, const Mat guess_disparity) {
	cv::Mat diff, unoccluded_mask;
	int num_pixel;
	std::tie(diff, unoccluded_mask, num_pixel) = get_unoccluded_diff(gold_disparity, guess_disparity);

	double score = 1.0/sqrt(num_pixel)* norm(diff,NORM_L2);
	return score;
}

double ErrorMetrics::get_bias_unoccluded (const Mat gold_disparity, const Mat guess_disparity) {
	cv::Mat diff, unoccluded_mask;
	int num_pixel;
	std::tie(diff, unoccluded_mask, num_pixel) = get_unoccluded_diff(gold_disparity, guess_disparity);

	double score = sum(diff)[0] / (double) num_pixel;

	return score;
}

double ErrorMetrics::get_bad_matching_all (const Mat gold_disparity, const Mat guess_disparity)  {
	int num_pixel = gold_disparity.rows * gold_disparity.cols;

	Mat abs_diff = abs(gold_disparity - guess_disparity);
	Mat is_bad = (abs_diff > EVAL_BAD_THRESH) / 255;
	double num_bad_pixel = sum(is_bad)[0];
	double score = num_bad_pixel/num_pixel;

	return score;
}


/*

	For unoccluded in both images

	x = guess
	y = gold

	RMSE = sqrt(mean((x - y)^2))
	mean bias = mean(x - y)
	R^2 = 1 - SSres / SStot
		SSres = sum((x - y)^2)
		SStot = sum((x - mean(x))^2)
	Correlation = (sum(x*y) - n*mean(x)*mean(y)) / ( (n - 1) * std(x) * std(y))

	occluded
	
	occluded_gold = (gold_disparity == 0);
	occluded_guess = (guess_disparity == 0);

	cv::countNonZero

*/