#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;


#ifndef STRUCT_TRACK
  #define STRUCT_TRACK

  // Structure of tracks
  struct track {
    vector<Point2f> p; // Position
    Rect rect;         // Bounding box
    Point2f v;         // Speed
    Point2f a;         // Acceleration
    int kfound=0;      // Number of frame track has been updated
    int klost=0;       // Number of frame track has been lost
    bool found=true;   // Found corresponding object at frame
    // Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255)); // Color
    Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255); // Color
  };

  // Structure of objects
  struct object {
    Point2f p; // Position
    Rect rect; // Bounding box
  };

#endif
