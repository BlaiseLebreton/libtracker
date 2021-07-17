#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;


#ifndef STRUCT_TRACK
  #define STRUCT_TRACK

  // Structure of parameters
  struct tracker_params {
    float distth  = 20;   // Distance threshold for new track
    int   klost   = 20;   // Number of frame after a track is deleted if lost

    // Coefficient for distance to :
    float c_cr    = 1.0;  // Center
    float c_tl    = 0.0;  // Top-left     corner
    float c_br    = 0.0;  // Bottom-right corner
  };

  // Structure of tracks
  struct track {
    vector<Point2f> p;                                                         // Position
    Rect            rect;                                                      // Bounding box
    Point2f         v;                                                         // Speed
    Point2f         a;                                                         // Acceleration
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
