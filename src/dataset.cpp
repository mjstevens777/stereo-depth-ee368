#include "stereo-dataset.h"
#include "middlebury.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

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
  for (int i = 1; i <= 3; i++) {
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