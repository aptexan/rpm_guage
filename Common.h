#pragma once
#include <math.h>
#include <string>

#define PI 3.1415926f

#define WIN_HT 500
#define WIN_WD 500

// Calculate x and y co-ords of Circle/Arc given the radian angle theta and the radius.
#define RADIAL_X(theta, r)  ((float)(r) * cosf(theta))
#define RADIAL_Y(theta, r)  ((r) * sinf(theta))

// Convert  degree to radian
#define RAD(deg) (((deg) * PI) / 180)

using DegType = float;

void Render(void);
void OnMouse(int button, int state, int x, int y);

// Draw an arc given:
// 1) start to end angle (anti-clockwise) in degrees and 2) the radius
//   0 (and 360) degree is on positive x-axis
//  90 on positive y-axis
// 180 on negative x-axis
// 270 on negative y-axis
void DrawArc(DegType deg_begin, DegType deg_end, float radius);

void DrawArcFilled(DegType deg_begin, DegType deg_end,
  float outer_radius, float inner_radius,
  float red, float green, float blue);

// Draw a line segment from a center/origin radiating outwards.
// give:
// deg_angle: the direction 
// seg_begin: starting distance from center
// seg_end  : ending distance from the center
// As an example, a needle for RPM gauge can be drawn as a radial segment.
void DrawRadialSegment(DegType deg_angle, float seg_begin, float seg_end);

void PrintTextXYPos(std::string str, float x, float y);
void PrintTextRadialPos(const std::string& str, DegType deg_angle, float pos);

float NormalizeX(int x);
float NormalizeY(int y);
