#include "stereo-dataset.h"
#include "algorithms.h"
#include "opencv2/highgui/highgui.hpp"
#include <cstdlib>
#include <ctime>

#define WINDOW_NAME "Stereo Pair"

int main(int argc, const char *argv[]) {
  StereoDataset dataset;
  srand (time(NULL));

  StereoPair pair = dataset.get_random_stereo_pair();

  NCCDisparity nd(9);
  nd.compute(pair);

  cv::namedWindow("Stereo Pair", cv::WINDOW_AUTOSIZE);
  cv::Mat im;
  pair.left.convertTo(im, CV_8UC3);
  cv::imshow("Stereo Pair", im);

  cv::waitKey(0);   
  cv::imshow("Stereo Pair", pair.disparity_left);

  cv::waitKey(0); 
  return 0;
}
