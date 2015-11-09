#include "stereo-dataset.h"
#include "opencv2/highgui/highgui.hpp"
#include "error-metrics.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#define WINDOW_NAME "Stereo Pair"

int main(int argc, const char *argv[]) {
  StereoDataset dataset;
  srand (time(NULL));

  StereoPair pair = dataset.get_random_stereo_pair();

  //Test rms
  double true_rms = ErrorMetrics::get_rms_error_all(pair.true_disparity_left,  pair.true_disparity_left); 
  double left_right_rms = ErrorMetrics::get_rms_error_all(pair.true_disparity_left,  pair.true_disparity_right); 
  cout << "True rms: " << true_rms << "\n";
  cout << "Left right rms: " << left_right_rms << "\n";

  //Test bad matching
  double true_bm = ErrorMetrics::get_bad_matching_all(pair.true_disparity_left,  pair.true_disparity_left); 
  double left_right_bm = ErrorMetrics::get_bad_matching_all(pair.true_disparity_left,  pair.true_disparity_right); 
  cout << "True bm: " << true_bm << "\n";
  cout << "Left right bm: " << left_right_bm << "\n";


  cv::namedWindow("Stereo Pair", cv::WINDOW_AUTOSIZE);
  cv::imshow("Stereo Pair", pair.left);

  cv::waitKey(0); 

  return 0;
}
