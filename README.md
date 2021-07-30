# LibTracker  
C++ library for tracking objects  

This library associates objects with tracks  
An object and a track are associated based on the ratio of their overlapping area and their respective areas : A&B/(A+B)  
If this ratio is too small, (ie inferior to `simith`) a new track is created on this object.  
If a track has no object associated to it, it will be erased after `klost` iterations.  
  
Developed mainly for https://github.com/BlaiseLebreton/CV_Cam
