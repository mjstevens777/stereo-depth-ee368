#include "stereo-dataset.h"
#include "middlebury.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

StereoPair::StereoPair(cv::Mat _left, cv::Mat _right,
    cv::Mat _true_left, cv::Mat _true_right,
    int _base_offset) :
  left(_left),
  right(_right),
  true_disparity_left(_true_left),
  true_disparity_right(_true_right),
  base_offset(_base_offset)
{
  rows = left.rows;
  cols = left.cols;
  left.convertTo(left, CV_32FC3); 
  right.convertTo(right, CV_32FC3);
  cvtColor(true_disparity_left, true_disparity_left, CV_BGR2GRAY);
  cvtColor(true_disparity_right, true_disparity_right, CV_BGR2GRAY);

  double mn, mx;
  cv::Mat nonzero = (true_disparity_left != 0);
  nonzero.convertTo(nonzero, CV_8U);
  minMaxLoc(true_disparity_left, &mn, &mx, NULL, NULL, nonzero);
  min_disparity_left = mn;
  max_disparity_left = mx;

  nonzero = (true_disparity_right != 0);
  nonzero.convertTo(nonzero, CV_8U);
  minMaxLoc(true_disparity_right, &mn, &mx, NULL, NULL, nonzero);
  min_disparity_right = mn;
  max_disparity_right = mx;

  return;
}

StereoPair StereoDataset::get_stereo_pair(const string dataset, int illumination, int exposure) {
  char path[1024];
  cout  << "Loading" << dataset << illumination << exposure << endl;
  snprintf(path, 1024, left_format, dataset.c_str(), illumination, exposure);
  Mat left = imread(path, CV_LOAD_IMAGE_COLOR);
  snprintf(path, 1024, right_format, dataset.c_str(), illumination, exposure);
  Mat right = imread(path, CV_LOAD_IMAGE_COLOR);
  snprintf(path, 1024, true_left_format, dataset.c_str());
  Mat true_left = imread(path, CV_LOAD_IMAGE_COLOR);
  snprintf(path, 1024, true_right_format, dataset.c_str());
  Mat true_right = imread(path, CV_LOAD_IMAGE_COLOR);

  snprintf(path, 1024, offset_format, dataset.c_str());
  ifstream offset_file(path);
  int base_offset;
  offset_file >> base_offset;

  return StereoPair(left, right, true_left, true_right, base_offset);
}

vector<string> StereoDataset::get_all_datasets() {
  vector<string> datasets;
  for (int i = 0; i < NumMiddleburyDatasets; i++) {
    datasets.push_back(string(MiddleburyDatasetNames[i]));
  }
  return datasets;
}
vector<int> StereoDataset::get_all_illuminations() {
  vector<int> illuminations;
  for (int i = 1; i <= 3; i++) {
    illuminations.push_back(i);
  }
  return illuminations;
}
vector<int> StereoDataset::get_all_exposures() {
  vector<int> exposures;
  for (int i = 0; i <= 2; i++) {
    exposures.push_back(i);
  }
  return exposures;
}

string StereoDataset::get_random_dataset() {
  return string(
    MiddleburyDatasetNames[
      rand() % NumMiddleburyDatasets
    ]);
}
int StereoDataset::get_random_illumination() {
  return (rand() % 3) + 1;
}
int StereoDataset::get_random_exposure() {
  return rand() % 3;  
}

StereoPair StereoDataset::get_random_stereo_pair() {
  return get_stereo_pair(
    get_random_dataset()
    , get_random_illumination()
    , get_random_exposure()
  );
}