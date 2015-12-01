#include "stereo-dataset.h"
#include "algorithms.h"
#include "error-metrics.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
  StereoDataset dataset;
  srand (time(NULL));

  if (argc < 3) {
    cerr << "Must enter scale and either ncc or gc" << endl;
    exit(1);
  }

  float scale = atof(argv[1]);

  string alg_name(argv[2]);


  bool use_gc;
  if (alg_name == "ncc") {
    use_gc = false;
  } else if (alg_name == "gc") {
    use_gc = true;
  } else {
    cerr << "Must enter either ncc or gc" << endl;
    exit(1);
  }

  int Cp = 0;
  int V = 0;
  int window_size = 0;

  if (use_gc) {
    if (argc < 5) {
      cerr << "Must enter Cp and V" << endl;
      exit(1);
    }
    Cp = atoi(argv[3]);
    V = atoi(argv[4]);
  } else {
    if (argc < 4) {
      cerr << "Must enter window size" << endl;
    }
    window_size = atoi(argv[3]);
  }

  for (string name : dataset.get_all_datasets()) {
    StereoPair pair = dataset.get_stereo_pair(name);
    pair.resize(0.5);
    stringstream ss;
    string base_name;
    if (use_gc) {
      GraphCutDisparity gc(Cp, V);
      gc.compute(pair);
      ss << "results/gc-" << pair.name
        << "-scale-" << scale
        << "-Cp-" << Cp << "-V-" << V;
      base_name = ss.str();
    } else {
      NCCDisparity ncc(window_size);
      ncc.compute(pair);
      ss << "results/ncc-" << pair.name
        << "-scale-" << scale
        << "-w-" << window_size;
      base_name = ss.str();
    }
    double rmse_left = ErrorMetrics::get_rms_error_unoccluded(pair.true_disparity_left, pair.disparity_left);
    double rmse_right = ErrorMetrics::get_rms_error_unoccluded(pair.true_disparity_right, pair.disparity_right);
    cout << base_name << endl;
    cout << "Left RMSE: " << rmse_left << endl;
    cout << "Right RMSE: " << rmse_right << endl;
  }
}


// Get the mean squared error for valid correspondences:
 // for (string name : dataset.get_all_datasets()) {
 //    for (int illum : dataset.get_all_illuminations()) {
 //      for (int expo : dataset.get_all_exposures()) {
 //        StereoPair pair = dataset.get_stereo_pair(name, illum, expo);
 //        double err_sum = 0;
 //        int count = 0;

 //        for (int i = 0; i < pair.rows; i++) {
 //          for (int j = 0; j < pair.cols; j++) {
 //            int d_left = pair.true_disparity_left.at<uchar>(i, j);
 //            if (d_left == 0) continue;
 //            int j2 = j - d_left;
 //            if (j2 < 0 || j2 >= pair.cols) continue;
 //            count++;
 //            cv::Vec3f col1 = pair.left.at<Vec3f>(i, j);
 //            cv::Vec3f col2 = pair.right.at<Vec3f>(i, j2);
 //            cv::Vec3f col_diff = col1 - col2;

 //            err_sum += cv::norm(col_diff) * cv::norm(col_diff);

 //          }
 //        }

 //        double mse = err_sum / (double) count;
 //        cout << mse << endl;
 //      }
 //    }
 //  }