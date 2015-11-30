#include "stereo-dataset.h"
#include "algorithms.h"
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

  for (string name : dataset.get_all_datasets()) {
    for (int illum : dataset.get_all_illuminations()) {
      for (int expo : dataset.get_all_exposures()) {
        StereoPair pair = dataset.get_stereo_pair(name, illum, expo);
        double err_sum = 0;
        int count = 0;

        for (int i = 0; i < pair.rows; i++) {
          for (int j = 0; j < pair.cols; j++) {
            int d_left = pair.true_disparity_left.at<uchar>(i, j);
            if (d_left == 0) continue;
            int j2 = j - d_left;
            if (j2 < 0 || j2 >= pair.cols) continue;
            count++;
            cv::Vec3f col1 = pair.left.at<Vec3f>(i, j);
            cv::Vec3f col2 = pair.right.at<Vec3f>(i, j2);
            cv::Vec3f col_diff = col1 - col2;

            err_sum += cv::norm(col_diff) * cv::norm(col_diff);

          }
        }

        double mse = err_sum / (double) count;
        cout << mse << endl;
      }
    }
  }

  StereoPair pair = dataset.get_random_stereo_pair();

  NCCDisparity nd(9);
  nd.compute(pair);

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
  cv::Mat im;
  pair.left.convertTo(im, CV_8UC3);
  cv::imshow("Stereo Pair", im);
  cv::waitKey(0);   

  pair.disparity_left.convertTo(im, CV_8U);
  cv::imshow("Stereo Pair", im);
  cv::waitKey(0); 

  pair.true_disparity_left.convertTo(im, CV_8U);
  cv::imshow("Stereo Pair", im);
  cv::waitKey(0);


  return 0;
}
