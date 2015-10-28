#include "stereo-dataset.h"
#include "opencv2/highgui/highgui.hpp"

#define WINDOW_NAME "Stereo Pair"

int main(int argc, const char *argv[]) {
  StereoDataset dataset;
  StereoPair pair = dataset.get_random_stereo_pair();

  cv::namedWindow("Stereo Pair", cv::WINDOW_AUTOSIZE);
  cv::imshow("Stereo Pair", pair.left);

  cv::waitKey(0); 
  return 0;
}
