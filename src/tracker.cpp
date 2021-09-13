#include "struct.h"

#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;

tracker_params TrackerParams;
vector<track>  Tracks;
vector<object> Objects;

// Evolution matrices
// double dt = 1.0/60.0;
double dt = 1.0;
Mat I = (Mat_<double>(S, S) << 1, 0, 0, 0, 0, 0,
                               0, 1, 0, 0, 0, 0,
                               0, 0, 1, 0, 0, 0,
                               0, 0, 0, 1, 0, 0,
                               0, 0, 0, 0, 1, 0,
                               0, 0, 0, 0, 0, 1);

Mat A = (Mat_<double>(S, S) << 1, 0, dt,  0, 1/2*dt*dt,         0,
                               0, 1,  0, dt,         0, 1/2*dt*dt,
                               0, 0,  1,  0,        dt,         0,
                               0, 0,  0,  1,         0,        dt,
                               0, 0,  0,  0,         1,         0,
                               0, 0,  0,  0,         0,         1);
Mat B =  Mat_<double>(S, S);
Mat C = I;

Mat U = Mat_<double>(S, 1);

// Noise matrices
Mat Rn = (Mat_<double>(S, S) <<  5e+1,      0,    0,    0,     0,     0,
                                    0,   5e+1,    0,    0,     0,     0,
                                    0,      0, 2e+1,    0,     0,     0,
                                    0,      0,    0, 2e+1,     0,     0,
                                    0,      0,    0,    0,  1e+1,     0,
                                    0,      0,    0,    0,     0,  1e+1);

Mat Rv = (Mat_<double>(S, S) <<  1e-1,    0,     0,     0,     0,     0,
                                    0, 1e-1,     0,     0,     0,     0,
                                    0,    0,  1e-2,     0,     0,     0,
                                    0,    0,     0,  1e-2,     0,     0,
                                    0,    0,     0,     0,  1e-3,     0,
                                    0,    0,     0,     0,     0,  1e-3);

// Set tracker parameters
void Tracker_SetParameters(double simith,
                           int    klost,
                           int    method) {

  TrackerParams.simith = simith;
  TrackerParams.klost  = klost;
  TrackerParams.method = method;
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
double Tracker_Similarity(object Obj, int trck) {

  double s = 0;

  // Area overlapping
  if (TrackerParams.method == 1) {
    // Ratio of overlapping
    Rect Inter   = Obj.rect & Tracks[trck].rect;

    // Areas
    double Area_I = Inter.width    * Inter.height    / 2.0;
    double Area_O = Obj.rect.width * Obj.rect.height / 2.0;
    double Area_T = Tracks[trck].rect.width * Tracks[trck].rect.height / 2.0;

    s = 2*Area_I/(Area_O+Area_T);
  }

  // Euclidian distance
  else if (TrackerParams.method == 2) {
    double dx = Obj.p.x - Tracks[trck].Xp.at<double>(0,0);
    double dy = Obj.p.y - Tracks[trck].Xp.at<double>(1,0);
    s  = 1.0/sqrt(dx*dx + dy*dy);
  }

  return s;
}

// Predict futur position of tracks
void Tracker_Predict() {
  for (int trck = 0; trck < Tracks.size(); trck++) {

    // Initialize it to not found
    Tracks[trck].obj = -1;

    // Predict new vector state
    Tracks[trck].Xp = A*Tracks[trck].Xr + B*U;

    // Predict new measure
    Tracks[trck].Zp = C*Tracks[trck].Xp;

    // Predict new uncertainty
    Tracks[trck].P = A*Tracks[trck].P*A.t() + Rv;

    // Move rect to predicted position
    Point2f center =          Point2f(Tracks[trck].Xr.at<double>(0,0), Tracks[trck].Xr.at<double>(1,0));
    Point2f tl     = center - Point2f(Tracks[trck].rect.width/2.0,     Tracks[trck].rect.height/2.0);
    Tracks[trck].rect.x = tl.x;
    Tracks[trck].rect.y = tl.y;
  }
}

// Associate Tracks with Objects
void Tracker_Associate() {
  for (int obj = 0; obj < Objects.size(); obj++) {
    double maxsimi = 0.0;
    int mtrck = -1;
    double simi;
    for (int trck = 0; trck < Tracks.size(); trck++) {

      // Measure similarity
      simi = Tracker_Similarity(Objects[obj], trck);

      // Save the one with the maximum similarity
      if (simi > maxsimi) {
        maxsimi = simi;
        mtrck = trck;
      }
    }

    // Create new track if no track match the object
    if ((maxsimi <= TrackerParams.simith) || (mtrck == -1)) {
      track NewElem;
      NewElem.p.push_back(Objects[obj].p);
      NewElem.Xr = 0;
      NewElem.P  = Rn;
      NewElem.Xr.at<double>(0,0) = Objects[obj].p.x;
      NewElem.Xr.at<double>(1,0) = Objects[obj].p.y;
      NewElem.rect = Objects[obj].rect;
      Tracks.push_back(NewElem);
    }

    // Else associate and update track
    else {

      // Update counters
      Tracks[mtrck].kfound++;
      Tracks[mtrck].klost = 0;
      Tracks[mtrck].obj   = obj;

    }
  }


  // Update counts
  for (int trck = 0; trck < Tracks.size(); trck++) {
    if (Tracks[trck].obj == -1) {
      Tracks[trck].klost++;
      Tracks[trck].kfound = 0;
    }
  }
}

void Tracker_Correct() {

  for (int trck = 0; trck < Tracks.size(); trck++) {
    if (Tracks[trck].obj != -1) {

      // Virtual sensors
      double dx  = Objects[Tracks[trck].obj].p.x - Tracks[trck].Xr.at<double>(0,0);
      double dy  = Objects[Tracks[trck].obj].p.y - Tracks[trck].Xr.at<double>(1,0);
      double dvx =                         dx/dt - Tracks[trck].Xr.at<double>(2,0);
      double dvy =                         dy/dt - Tracks[trck].Xr.at<double>(3,0);

      // Create measure
      Mat Zr = (Mat_<double>(6, 1) << Objects[Tracks[trck].obj].p.x,
                                      Objects[Tracks[trck].obj].p.y,
                                      dx/dt,
                                      dy/dt,
                                      dvx/dt,
                                      dvy/dt);

      // Kalman Gain
      Tracks[trck].K  = Tracks[trck].P*C.t()*(C*Tracks[trck].P*C.t() + Rn).inv();

      // Correction
      Tracks[trck].Xr = Tracks[trck].Xr + Tracks[trck].K*(Zr - Tracks[trck].Zp);

      // Uncertainty
      Tracks[trck].P  = (I - Tracks[trck].K*C)*Tracks[trck].P;

      // Update position
      Tracks[trck].p.push_back(Point2f(
        Tracks[trck].Xr.at<double>(0,0),
        Tracks[trck].Xr.at<double>(1,0)
      ));


      Point2f center =          Point2f(Tracks[trck].Xr.at<double>(0,0), Tracks[trck].Xr.at<double>(1,0));
      Point2f tl     = center - Point2f(Tracks[trck].rect.width/2.0,     Tracks[trck].rect.height/2.0);
      Tracks[trck].rect   = Objects[Tracks[trck].obj].rect;
      Tracks[trck].rect.x = tl.x;
      Tracks[trck].rect.y = tl.y;
    }
  }
}

// Resample tracks
int Tracker_Resample() {

  vector<track> Tracks2;
  for (int trck = 0; trck < Tracks.size(); trck++) {
    if (Tracks[trck].klost <= TrackerParams.klost || Tracks[trck].obj != -1) {
      Tracks2.push_back(Tracks[trck]);
    }
  }
  Tracks = Tracks2;

  return Tracks.size();
}

// Draw uncertainty around predicted position
void DrawUncertainty(Mat img, int trck) {

  // Get x/y uncertainty only
  Mat Pxy  = Mat_<double>(2, 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      Pxy.at<double>(i,j) = Tracks[trck].P.at<double>(i,j);
    }
  }

  Mat eigenvalues;
  Mat eigenvectors;
  eigen(Pxy, eigenvalues, eigenvectors); // TODO : Need a way to keep the order of the eigenvals

  double sum = 0;
  for (int i = 0; i < 2; i++) {
    sum += eigenvalues.at<double>(i,0);
  }
  if (sum < 0) {
    for (int i = 0; i < 2; i++) {
      eigenvalues.at<double>(i,0) = abs(eigenvalues.at<double>(i,0));
    }
  }

  Point2f center = Point2f(Tracks[trck].Xr.at<double>(0,0), Tracks[trck].Xr.at<double>(1,0));

  Size axes;
  // if (isnan(eigenvalues.at<double>(0,0)) || isnan(eigenvalues.at<double>(0,0))) {
    axes.width  = 25;
    axes.height = 50;
  // }
  // else {
    // axes.width  = (int)3*sqrt(eigenvalues.at<double>(1,0))+1;
    // axes.height = (int)3*sqrt(eigenvalues.at<double>(0,0))+1;
  // }


  double angle = -atan2(eigenvectors.at<double>(0,0), eigenvectors.at<double>(1,0))*180/M_PI;

  ellipse(img, center, axes, angle, 0, 360, Tracks[trck].color, 1, 1);
}

// Draw tracks
void Tracker_DrawTracks(Mat img) {
  for (int trck = 0; trck < Tracks.size(); trck++) {

    // Bounding box
    rectangle(img, Tracks[trck].rect.tl(), Tracks[trck].rect.br(), Tracks[trck].color, 1);

    // Center
    circle(img, Tracks[trck].p.back(), 1, Tracks[trck].color, 2);

    // Draw ellipse of uncertainty
    DrawUncertainty(img, trck);

    // Trajectory
    for (int ip = 1; ip < Tracks[trck].p.size(); ip++) {
      line(img, Tracks[trck].p[ip-1], Tracks[trck].p[ip], Tracks[trck].color, 2);
    }
  }
}

// Draw objects
void Tracker_DrawObjects(Mat img) {
  Scalar color = Scalar(0,255,0);

  for (int obj = 0; obj < Objects.size(); obj++) {

    // Bounding box
    rectangle(img, Objects[obj].rect.tl(), Objects[obj].rect.br(), color, 1);

    // Center
    circle(img, Objects[obj].p, 1, color, 2);
  }
}

// Get Tracks
vector<track> Tracker_GetTracks() {
  return Tracks;
}
