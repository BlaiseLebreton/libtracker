#include "struct.h"

#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;

tracker_params TrackerParams;
vector<track>  Tracks;
vector<object> Objects;

// Set tracker parameters
void Tracker_SetParameters(float simith,
                           int   klost,
                           float c_cr,
                           float c_tl,
                           float c_br) {

  TrackerParams.simith = simith;
  TrackerParams.klost  = klost;
  TrackerParams.c_cr   = c_cr;
  TrackerParams.c_tl   = c_tl;
  TrackerParams.c_br   = c_br;
}

// Clear all objects
void Tracker_ClearObjects() {
  Objects.clear();
}

// Add object
void Tracker_AddObject(Rect rect_obj) {
  Objects.push_back({
    (rect_obj.tl() + rect_obj.br())/2.0,
    rect_obj
  });
}

// Measure similarity between track and object
float Tracker_Similarity(object Obj, track Trk) {

  // Ratio of overlapping
  Rect Inter   = Obj.rect & Trk.rect;

  // Areas
  float Area_I = Inter.width    * Inter.height    / 2.0;
  float Area_O = Obj.rect.width * Obj.rect.height / 2.0;
  float Area_T = Trk.rect.width * Trk.rect.height / 2.0;

  // Similarity (if same then s = 1, if not overlapping then 0)
  float s = 2*Area_I/(Area_O+Area_T);

  return s;
}

// Predict futur position of tracks
void Tracker_PredictTracks() {
  for (int trck = 0; trck < Tracks.size(); trck++) {

    // Initialize it to not found
    Tracks[trck].found = false;

    // Predict new position of tracks / TODO : Predict rectangle as-well
    Tracks[trck].a = Tracks[trck].a;
    Tracks[trck].v = Tracks[trck].v + 1/2*Tracks[trck].a;
    Tracks[trck].p.push_back(Tracks[trck].p.back() + Tracks[trck].v);

    // Move rect
    Tracks[trck].rect.x = Tracks[trck].p.back().x;
    Tracks[trck].rect.y = Tracks[trck].p.back().y;
  }
}

// Associate Tracks with Objects
void Tracker_Associate(Mat display) {
  for (int obj = 0; obj < Objects.size(); obj++) {
    float maxsimi = 0.0;
    int maxtrck = -1;
    float simi;
    for (int trck = 0; trck < Tracks.size(); trck++) {

      // Measure similarity
      simi = Tracker_Similarity(Objects[obj], Tracks[trck]);

      // Save the one with the maximum similarity
      if (simi > maxsimi) {
        maxsimi = simi;
        maxtrck = trck;
      }
    }

    // Create new track if no track match the object
    if ((maxsimi <= TrackerParams.simith) || (maxtrck == -1)) {
      track NewElem;
      NewElem.p.push_back(Objects[obj].p);
      NewElem.rect = Objects[obj].rect;
      Tracks.push_back(NewElem);
    }

    // Else associate and update track
    else {

      // Update counters
      Tracks[maxtrck].kfound++;
      Tracks[maxtrck].klost = 0;
      Tracks[maxtrck].found = true;

      // Update track position
      Tracks[maxtrck].p.back() = Objects[obj].p; // TODO : Bayesian filter
      Tracks[maxtrck].rect     = Objects[obj].rect;

      // Calculate new speed and acceleration
      Tracks[maxtrck].v = Tracks[maxtrck].p.end()[-1] - Tracks[maxtrck].p.end()[-2];
      Tracks[maxtrck].a = (Tracks[maxtrck].p.end()[-1] - Tracks[maxtrck].p.end()[-2]) - (Tracks[maxtrck].p.end()[-2] - Tracks[maxtrck].p.end()[-3]);
    }
  }


  // Update counts
  for (int trck = 0; trck < Tracks.size(); trck++) {
    if (!Tracks[trck].found) {
      Tracks[trck].klost++;
      Tracks[trck].kfound = 0;   // Pas sur
    }
  }
}
// Resample tracks
int Tracker_ResampleTracks() {

  vector<track> Tracks2;
  for (int trck = 0; trck < Tracks.size(); trck++) {
    if (Tracks[trck].klost <= TrackerParams.klost || Tracks[trck].found) {
      Tracks2.push_back(Tracks[trck]);
    }
  }
  Tracks = Tracks2;

  return Tracks.size();
}

// Draw tracks
void Tracker_DrawTracks(Mat img) {
  for (int trck = 0; trck < Tracks.size(); trck++) {

    // Bounding box
    rectangle(img, Tracks[trck].rect.tl(), Tracks[trck].rect.br(), Tracks[trck].color, 1);

    // Center
    circle(img, Tracks[trck].p.back(), 1, Tracks[trck].color, 2);

    // Velocity vector
    arrowedLine(img, Tracks[trck].p.back(), Tracks[trck].p.back() + Tracks[trck].v*10, Tracks[trck].color, 1, 8, 0, 0.1);

    // Debug
    // putText    (img, to_string(trck),                Tracks[trck].p.back() + Point2f(0,0),  FONT_HERSHEY_DUPLEX, 0.5, Scalar(255,255,255), 1);
    // putText    (img, to_string(Tracks[trck].kfound), Tracks[trck].p.back() + Point2f(0,10), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255,255,255), 1);
    // putText    (img, to_string(Tracks[trck].klost),  Tracks[trck].p.back() + Point2f(0,20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255,255,255), 1);

    // Trajectory
    for (int ip = 1; ip < Tracks[trck].p.size(); ip++) {
      line(img, Tracks[trck].p[ip-1], Tracks[trck].p[ip], Tracks[trck].color, 2);
    }
  }
}

// Get Tracks
vector<track> Tracker_GetTracks() {
  return Tracks;
}
