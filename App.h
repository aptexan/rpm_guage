#pragma once
#include "Types.h"
#include "ThrottleWidget.h"

#include <string>

class App {
public:
  void InitGL(int argc, char** argv);
  void Render();
  void OnMouse(int button, int state, int x, int y);

private:
  // Draw an arc given:
  // 1) start to end angle (anti-clockwise) in degrees and 2) the radius
  //   0 (and 360) degree is on positive x-axis
  //  90 on positive y-axis
  // 180 on negative x-axis
  // 270 on negative y-axis
  void DrawArc(DegType deg_begin, DegType deg_end, float radius);

  // Draw a line segment from a center/origin radiating outwards.
  // give:
  // deg_angle: the direction 
  // seg_begin: starting distance from center
  // seg_end  : ending distance from the center
  // As an example, a needle for RPM gauge can be drawn as a radial segment.
  void DrawRadialSegment(DegType deg_angle, float seg_begin, float seg_end);

  void PrintTextXYPos(std::string str, float x, float y);

  void PrintTextRadialPos(const std::string& str, DegType deg_angle, float pos);

  void DrawReferenceGrid_2B_deleted();

  float NormalizeX(int x);
  float NormalizeY(int y);

  void DrawDial();

private:
  ThrottleWidget _throttle;
};