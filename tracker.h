#include "struct.h"

// Set tracker parameters
void Tracker_SetParameters(double simith,
                           int    klost,
                           int    method);

// Clear all objects
void Tracker_ClearObjects();

// Add object
void Tracker_AddObject(Rect rect_obj);

// Measure distance between track and object
double Tracker_Similarity(object Obj, track Trk);

// Predict futur position of tracks
void Tracker_PredictTracks();

// Associate Tracks with Objects
void Tracker_Associate();

// Resample tracks
int Tracker_ResampleTracks();

// Draw uncertainty ellipse
void DrawUncertainty(Mat img, track Trk);

// Draw tracks
void Tracker_DrawTracks(Mat img);

// Draw Objects
void Tracker_DrawObjects(Mat img);

// Get Tracks
vector<track> Tracker_GetTracks();
