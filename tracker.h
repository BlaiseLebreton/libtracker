#include "struct.h"

// Clear all objects
void Tracker_ClearObjects();

// Add object
void Tracker_AddObject(Rect rect_obj);

// Measure distance between track and object
float Tracker_Distance(object Obj, track Trck);

// Predict futur position of tracks
void Tracker_PredictTracks();

// Associate Tracks with Objects
void Tracker_Associate();

// Draw tracks
void Tracker_DrawTracks(Mat img);

// Resample tracks
int Tracker_ResampleTracks();
