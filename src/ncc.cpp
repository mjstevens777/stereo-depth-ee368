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

int NCCDisparity::best_location(cv::Mat t, cv::Mat row, cv::Mat magnitude) {
  vector<cv::Mat> row_rgb(3);
  vector<cv::Mat> t_rgb(3);
  vector<cv::Mat> detections_rgb(3);

  split(row, row_rgb);
  split(row, detections_rgb);
  split(t, t_rgb);

  cv::namedWindow("Best Location", cv::WINDOW_AUTOSIZE);
  cv::Mat im;

  for (int c = 0; c < 3; c++) {
    // subtract mean
    cv::Scalar mean = cv::mean(t_rgb[c]);

    cv::subtract(t_rgb[c], mean, t_rgb[c]);

    // cv::normalize(t_rgb[c], im, 0, 1, cv::NORM_MINMAX);
    // cv::resize(im, im, cv::Size(0, 0), 3, 3);
    // cv::imshow("Best Location", im);
    // cv::waitKey(0);

    // cv::normalize(row_rgb[c], im, 0, 1, cv::NORM_MINMAX);
    // cv::resize(im, im, cv::Size(0, 0), 3, 3);
    // cv::imshow("Best Location", im);
    // cv::waitKey(0);

    cv::filter2D(row_rgb[c], detections_rgb[c], -1, t_rgb[c]);

    // cv::normalize(detections_rgb[c], im, 0, 1, cv::NORM_MINMAX);
    // cv::resize(im, im, cv::Size(0, 0), 3, 3);
    // cv::imshow("Best Location", im);
    // cv::waitKey(0);
  }

  cv::Mat detections;
  merge(detections_rgb, detections);
  int r = (window_size - 1) /  2;
  cv::Rect roi(0, r, pair->cols, 1);

  // cv::normalize(row, im, 0, 1, cv::NORM_MINMAX);
  // cv::resize(im, im, cv::Size(0, 0), 3, 3);
  // cv::imshow("Best Location", im);
  // cv::waitKey(0);


  cv::Mat magnitude_roi;
  magnitude(roi).copyTo(magnitude_roi);
  cv::cvtColor(detections(roi), detections, CV_BGR2GRAY);
  cv::cvtColor(magnitude_roi, magnitude_roi, CV_BGR2GRAY);

  // cv::normalize(detections, im, 0, 1, cv::NORM_MINMAX);
  // cv::resize(im, im, cv::Size(0, 0), 3, 20);
  // cv::imshow("Best Location", im);
  // cv::waitKey(0);

  // cv::normalize(magnitude, im, 0, 1, cv::NORM_MINMAX);
  // cv::resize(im, im, cv::Size(0, 0), 3, 3);
  // cv::imshow("Best Location", im);
  // cv::waitKey(0);  

  cv::divide(detections, magnitude_roi, detections);

  // cv::normalize(detections, im, 0, 1, cv::NORM_MINMAX);
  // cv::resize(im, im, cv::Size(0, 0), 1, 20);
  // cv::imshow("Best Location", im);
  // cv::waitKey(0);  

  cv::Point maxLoc;
  cv::minMaxLoc(detections, NULL, NULL, NULL, &maxLoc);

  return maxLoc.x;
}

cv::Mat NCCDisparity::get_magnitude(cv::Mat im) {
  cv::Mat im_sq;
  cv::pow(im, 2, im_sq);
  cv::Mat mean, mean_sq;

  cv::boxFilter(im, mean, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), false, cv::BORDER_CONSTANT);
  cv::boxFilter(im_sq, mean_sq, -1,
    cv::Size(window_size, window_size), cv::Point(-1,-1), false, cv::BORDER_CONSTANT);

  cv::Mat cross;
  cv::multiply(im, mean, cross);
  cv::multiply(cross, cv::Scalar(2), cross);

  cv::Mat out;
  cv::subtract(im_sq, cross, out);
  cv::add(out, mean_sq, out);

  return out;
}

NCCDisparity& NCCDisparity::compute(StereoPair &_pair) {
  pair = &_pair;

  cv::resize(pair->left, pair->left, cv::Size(0, 0), 0.3, 0.3);
  cv::resize(pair->right, pair->right, cv::Size(0, 0), 0.3, 0.3);
  cv::resize(pair->true_disparity_left, pair->true_disparity_left, cv::Size(0, 0), 0.3, 0.3);
  cv::resize(pair->true_disparity_right, pair->true_disparity_right, cv::Size(0, 0), 0.3, 0.3);
  pair->rows = pair->left.rows;
  pair->cols = pair->left.cols;

  pair->disparity_left = cv::Mat(pair->rows, pair->cols, CV_16S);
  pair->disparity_right = cv::Mat(pair->rows, pair->cols, CV_16S);

  cv::Mat magnitude_left = get_magnitude(pair->left);
  cv::Mat magnitude_right = get_magnitude(pair->right);

  int r = (window_size- 1) / 2;
  for (int i = r; i < (pair->rows - r); i++) {
    cout << i << endl;
    cv::Mat row_left = get_row(i, pair->left);
    cv::Mat row_right = get_row(i, pair->right);
    cv::Mat mag_row_left = get_row(i, magnitude_left);
    cv::Mat mag_row_right = get_row(i, magnitude_right);
    for (int j = r; j < (pair->rows - r); j++) {
      cv::Mat t_left = get_template(i, j, true);
      // cv::Mat t_right = get_template(i, j, false);
      int d_left = best_location(t_left, row_right, mag_row_right) - j - pair->base_offset;
      // int d_right = j - best_location(t_right, row_left, mag_row_left);
      pair->disparity_left.at<uchar>(i, j) = d_left;
      // pair->disparity_right.at<uchar>(i, j) = d_right;
    }
  }

  return *this;
}
