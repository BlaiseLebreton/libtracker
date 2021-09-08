#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;

#define S 6
#ifndef STRUCT_TRACK
  #define STRUCT_TRACK

  // Structure of parameters
  struct tracker_params {
    float simith  = 0.1;   // Similarity threshold for new track
    int   klost   = 20;   // Number of frame after a track is deleted if lost

    // Method to calculate similarity
    int method;  // 1 : Area overlapping
                 // 2 : Euclidian distance
  };

  // Structure of tracks
  struct track {
    Mat Xr = Mat_<double>(S, 1); // Real vector state
    Mat Xp = Mat_<double>(S, 1); // Predicted vector state
    Mat Zp = Mat_<double>(S, 1); // Predicted measure
    Mat P  = Mat_<double>(S, S); // Covariance
    Mat K  = Mat_<double>(S, S); // Kalman gain

    vector<Point2f> p;                                                         // Position history
    Rect            rect;                                                      // Bounding box
    int             kfound = 0;                                                // Number of frame track has been updated
    int             klost  = 0;                                                // Number of frame track has been lost
    bool            found  = true;                                             // Found corresponding object at frame
    Scalar          color  = Scalar(rand() % 255, rand() % 255, rand() % 255); // Color
  };

  // Structure of objects
  struct object {
    Point2f p; // Position
    Rect rect; // Bounding box
  };

#endif
