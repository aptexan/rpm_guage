#pragma once


#define PI 3.1415926f

#define WIN_HT 500
#define WIN_WD 500

// Calculate x and y co-ords of Circle/Arc given the radian angle theta and the radius.
#define RADIAL_X(theta, r)  ((r) * cosf(theta))
#define RADIAL_Y(theta, r)  ((r) * sinf(theta))

// Convert  degree to radian
#define RAD(deg) (((deg) * PI) / 180)

using DegType = float;

void Render(void);
void OnMouse(int button, int state, int x, int y);

