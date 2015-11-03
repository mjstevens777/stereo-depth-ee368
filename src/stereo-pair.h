#pragma once

#include "opencv2/core/core.hpp"
#include <string>

class StereoPair {
public:
  cv::Mat left, right;
  cv::Mat true_disparity_left, true_disparity_right;
  cv::Mat disparity_left, disparity_right;
  int base_offset;

  StereoPair(cv::Mat _left, cv::Mat _right,
    cv::Mat _true_left, cv::Mat _true_right,
    int _base_offset) :
  left(_left),
  right(_right),
  true_disparity_left(_true_left),
  true_disparity_right(_true_right),
  base_offset(_base_offset)
  {
    return;
  }
};