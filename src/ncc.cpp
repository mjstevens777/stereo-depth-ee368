#include "ncc.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>

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

  // subtract mean
  cv::Scalar mean = cv::mean(t);

  cv::subtract(t, mean, t);

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

int NCCDisparity::best_location(cv::Mat t, cv::Mat row, cv::Mat magnitude) {
  vector<cv::Mat> row_rgb(3);
  vector<cv::Mat> t_rgb(3);
  vector<cv::Mat> detections_rgb(3);

  split(row, row_rgb);
  split(row, detections_rgb);
  split(t, t_rgb);

  for (int c = 0; c < 3; c++) {
    cv::filter2D(row_rgb[c], detections_rgb[c], -1, t_rgb[c]);
  }

  cv::Mat detections;
  merge(detections_rgb, detections);
  int r = (window_size - 1) /  2;
  cv::Rect roi(0, r, pair->cols, 1);

  cv::divide(detections(roi), magnitude(roi), detections);

  cv::cvtColor(detections, detections, CV_BGR2GRAY);

  cv::Point maxLoc;
  cv::minMaxLoc(detections, NULL, NULL, NULL, &maxLoc);

  return maxLoc.x;
}

NCCDisparity& NCCDisparity::compute(StereoPair &_pair) {
  pair = &_pair;

  cv::resize(pair->left, pair->left, cv::Size(0, 0), 0.3, 0.3);
  cv::resize(pair->right, pair->right, cv::Size(0, 0), 0.3, 0.3);
  pair->rows = pair->left.rows;
  pair->cols = pair->left.cols;

  pair->disparity_left = cv::Mat(pair->rows, pair->cols, CV_8U);
  pair->disparity_right = cv::Mat(pair->rows, pair->cols, CV_8U);

  cv::Mat magnitude_left;
  cv::Mat magnitude_right;
  cv::pow(pair->left, 2, magnitude_left);
  cv::pow(pair->right, 2, magnitude_right);
  cv::boxFilter(magnitude_left, magnitude_left, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), false, cv::BORDER_CONSTANT);
  cv::boxFilter(magnitude_right, magnitude_right, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), false, cv::BORDER_CONSTANT);
  cv::add(magnitude_left, cv::Scalar(0.01), magnitude_left);
  cv::add(magnitude_right, cv::Scalar(0.01), magnitude_right);


  int r = (window_size- 1) / 2;
  for (int i = r; i < (pair->rows - r); i++) {
    cout << i << endl;
    cv::Mat row_left = get_row(i, pair->left);
    cv::Mat row_right = get_row(i, pair->right);
    cv::Mat mag_row_left = get_row(i, magnitude_left);
    cv::Mat mag_row_right = get_row(i, magnitude_right);
    for (int j = r; j < (pair->rows - r); j++) {
      cv::Mat t_left = get_template(i, j, true);
      cv::Mat t_right = get_template(i, j, false);
      int d_left = best_location(t_left, row_right, mag_row_right) - j;
      int d_right = j - best_location(t_right, row_left, mag_row_left);
      pair->disparity_left.at<uchar>(i, j) = d_left;
      pair->disparity_right.at<uchar>(i, j) = d_right;
    }
  }

  return *this;
}
