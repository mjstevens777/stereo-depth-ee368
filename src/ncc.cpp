#include "ncc.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include "opencv2/highgui/highgui.hpp"

using namespace std;

cv::Mat NCCDisparity::get_template(int i, int j, bool left) {
  int r = (window_size - 1) /  2;
  int min_i = i - r;
  int min_j = j - r;

  cv::Mat t;

  cv::Rect roi(min_j, min_i, window_size, window_size);

  if (left) {
    pair->left(roi).copyTo(t);
  } else {
    pair->right(roi).copyTo(t);
  }

  return t;
}


cv::Mat NCCDisparity::get_row(int i, cv::Mat im) {
  int r = (window_size - 1) /  2;
  int min_i = i - r;

  cv::Mat row;

  cv::Rect roi(0, min_i, pair->cols, window_size);

  im(roi).copyTo(row);

  return row;
}

int NCCDisparity::disparity(cv::Mat t, cv::Mat row, cv::Mat magnitude, int j, bool left) {
  vector<cv::Mat> row_rgb(3);
  vector<cv::Mat> t_rgb(3);
  vector<cv::Mat> detections_rgb(3);

  int r = (window_size - 1) /  2;

  int min_j, max_j;
  if (left) {
    // right = left - disparity
    min_j = j - pair->max_disparity_left - r;
    max_j = j - pair->min_disparity_left + r;
  } else {
    // left = right + disparity
    min_j = j + pair->min_disparity_right - r;
    max_j = j + pair->max_disparity_right + r;
  }

  if (min_j < 0) min_j = 0;
  if (max_j < 0) max_j = 0;
  if (min_j >= pair->cols) min_j = pair->cols - 1;
  if (max_j >= pair->cols) max_j = pair->cols - 1;

  int bounds_width = max_j - min_j + 1;
  if (bounds_width < window_size)
    return 0;

  cv::Rect bounded_roi(min_j, 0, bounds_width, window_size);  

  row = row(bounded_roi);
  magnitude = magnitude(bounded_roi);

  split(row, row_rgb);
  split(row, detections_rgb);
  split(t, t_rgb);

  for (int c = 0; c < 3; c++) {
    // subtract mean
    cv::Scalar mean = cv::mean(t_rgb[c]);
    cv::subtract(t_rgb[c], mean, t_rgb[c]);
    cv::filter2D(row_rgb[c], detections_rgb[c], -1, t_rgb[c]);
  }

  cv::Mat detections;
  merge(detections_rgb, detections);
  cv::Rect roi(0, r, bounds_width - window_size + 1, 1);

  cv::Mat magnitude_roi;
  magnitude(roi).copyTo(magnitude_roi);
  detections(roi).copyTo(detections);

  cv::divide(detections, magnitude_roi, detections);
  cv::cvtColor(detections, detections, CV_BGR2GRAY);

  // cv::normalize(detections, im, 0, 1, cv::NORM_MINMAX);
  // cv::resize(im, im, cv::Size(0, 0), 3, 20);
  // cv::imshow("Best Location", im);
  // cv::waitKey(0);  

  cv::Point maxLoc;
  cv::minMaxLoc(detections, NULL, NULL, NULL, &maxLoc);

  int max_loc_orig = maxLoc.x + min_j + r;

  // disparity = left - right
  if (left) {
    return j - max_loc_orig;
  } else {
    return max_loc_orig - left;
  }
}


// std = (sum(x^2) - sum(x)^2/n)/n
cv::Mat NCCDisparity::get_magnitude(cv::Mat im) {
  cv::Mat im_sq, mean_sq; // mean of x^2
  cv::pow(im, 2, im_sq);
  cv::boxFilter(im_sq, mean_sq, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), true, cv::BORDER_CONSTANT);

  cv::Mat mean, sq_mean; // (mean of x)^2
  cv::boxFilter(im, mean, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), true, cv::BORDER_CONSTANT);
  cv::pow(mean, 2, sq_mean);

  cv::Mat var, std_dev;
  // var = mean of x^2 - (mean of x)^2
  cv::subtract(mean_sq, sq_mean, var);
  cv::sqrt(var, std_dev);

  return std_dev;
}

NCCDisparity& NCCDisparity::compute(StereoPair &_pair) {
  pair = &_pair;

  // cv::resize(pair->left, pair->left, cv::Size(0, 0), 0.3, 0.3);
  // cv::resize(pair->right, pair->right, cv::Size(0, 0), 0.3, 0.3);
  // cv::resize(pair->true_disparity_left, pair->true_disparity_left, cv::Size(0, 0), 0.3, 0.3);
  // cv::resize(pair->true_disparity_right, pair->true_disparity_right, cv::Size(0, 0), 0.3, 0.3);
  // pair->rows = pair->left.rows;
  // pair->cols = pair->left.cols;

  pair->disparity_left = cv::Mat(pair->rows, pair->cols, CV_8U);
  pair->disparity_right = cv::Mat(pair->rows, pair->cols, CV_8U);

  cv::Mat magnitude_left = get_magnitude(pair->left);
  cv::Mat magnitude_right = get_magnitude(pair->right);

  int r = (window_size- 1) / 2;
  for (int i = r; i < (pair->rows - r); i++) {
    if ((i % 20) == 0)
      cout << i << endl;
    cv::Mat row_left = get_row(i, pair->left);
    cv::Mat row_right = get_row(i, pair->right);
    cv::Mat mag_row_left = get_row(i, magnitude_left);
    cv::Mat mag_row_right = get_row(i, magnitude_right);

    for (int j = r; j < (pair->rows - r); j++) {
      cv::Mat t_left = get_template(i, j, true);
      cv::Mat t_right = get_template(i, j, false);

      int d_left = disparity(t_left, row_right, mag_row_right, j, true);
      int d_right = disparity(t_right, row_left, mag_row_left, j, false);

      pair->disparity_left.at<uchar>(i, j) = d_left;
      pair->disparity_right.at<uchar>(i, j) = d_right;
    }
  }

  return *this;
}
