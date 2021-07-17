#include "struct.h"

#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;

tracker_params TrackerParams;
vector<track>  Tracks;
vector<object> Objects;

// Set tracker parameters
void Tracker_SetParameters(float distth,
                           int   klost,
                           float c_cr,
                           float c_tl,
                           float c_br) {

  TrackerParams.distth = distth;
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

// Measure distance between track and object
float Tracker_Distance(object Obj, track Trck) {
  float d = 0;

  Point2f dcr = Obj.p         - Trck.p.back();
  Point2f dtl = Obj.rect.tl() - Trck.rect.tl();
  Point2f dbr = Obj.rect.br() - Trck.rect.br();

  d += TrackerParams.c_cr*sqrt(abs(dcr.x*dcr.x + dcr.y*dcr.y));
  d += TrackerParams.c_tl*sqrt(abs(dtl.x*dtl.x + dtl.y*dtl.y));
  d += TrackerParams.c_br*sqrt(abs(dbr.x*dbr.x + dbr.y*dbr.y));

  return d;
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
  }
}

// Associate Tracks with Objects
void Tracker_Associate() {
  for (int obj = 0; obj < Objects.size(); obj++) {
    float mindist = 1000000;
    int mintrck = -1;
    float dist;
    for (int trck = 0; trck < Tracks.size(); trck++) {

      // Measure similarity
      dist = Tracker_Distance(Objects[obj], Tracks[trck]);

      // Save the one with the minimum distance
      if (dist < mindist) {
        mindist = dist;
        mintrck = trck;
      }
    }

    // Create new track if no track match the object
    if ((mindist > TrackerParams.distth) || (mintrck == -1)) {
      track NewElem;
      NewElem.p.push_back(Objects[obj].p);
      NewElem.rect = Objects[obj].rect;
      Tracks.push_back(NewElem);
    }

    // Else associate and update track
    else {

      // Update counters
      Tracks[mintrck].kfound++;
      Tracks[mintrck].klost = 0;
      Tracks[mintrck].found = true;

      // Update track position
      Tracks[mintrck].p.back() = Objects[obj].p; // TODO : Bayesian filter
      Tracks[mintrck].rect     = Objects[obj].rect;

      // Calculate new speed and acceleration
      Tracks[mintrck].v = Tracks[mintrck].p.end()[-1] - Tracks[mintrck].p.end()[-2];
      Tracks[mintrck].a = (Tracks[mintrck].p.end()[-1] - Tracks[mintrck].p.end()[-2]) - (Tracks[mintrck].p.end()[-2] - Tracks[mintrck].p.end()[-3]);
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
