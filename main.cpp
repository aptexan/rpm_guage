/*
Candidate : Prasad Apte
This installment: 
  - How to use Visual Studio for OPenGL
  - Some OpenGL basics.
  - Figured out how to obtain and setup GLUT library.
  - Functions for drawing Arc/Circle and line segments.
  - Builds and runs the app, displaying the RPM guage dial and the needle.
  - Functions for positioning and printing Text.
  - Changed DegType from int to float (was causing skewed minor slot markings).
NEXT: Figure out how to use/code to interact with mouse/keyboard 
    and user input controls.
*/

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

#include <iostream>
#include <string>

#define PI 3.1415926f

// Calculate x and y co-ords of circle given the radian angle theta and the radius.
#define X(theta, r)  ((r) * cosf(theta))
#define Y(theta, r)  ((r) * sinf(theta))

// Convert  degree to radian
#define RAD(deg) (((deg) * PI) / 180)

using DegType = float;

// Draw an arc given:
// 1) start to end angle (anti-clockwise) in degrees and 2) the radius
//   0 (and 360) degree is on positive x-axis
//  90 on positive y-axis
// 180 on negative x-axis
// 270 on negative y-axis
void DrawArc(DegType deg_begin, DegType deg_end, float radius,
              float origin_x = 0.0, float origin_y = 0.0) {
  int deg_steps{};
  if (deg_begin < deg_end)  {
    deg_steps = deg_end - deg_begin + 1;
  } else {
    deg_steps = (360 - deg_begin) + deg_end + 1;
  }

  glBegin(GL_LINE_STRIP);
  for (DegType deg = deg_begin; deg_steps > 0; ++deg, --deg_steps) {
    deg = deg > 360 ? 1 : deg;
    float x = X(RAD(deg), radius);
    float y = Y(RAD(deg), radius);
    glVertex2f(x, y);
  }
  glEnd();
}

// Draw a line segment from a center/origin radiating outwards.
// give:
// deg_angle: the direction 
// seg_begin: starting distance from center
// seg_end  : ending distance from the center
// As an example, a needle for RPM gauge can be drawn as a radial segment.
void DrawRadialSegment(DegType deg_angle, float seg_begin, float seg_end,
                        float origin_x = 0.0, float origin_y = 0.0) {
  glBegin(GL_LINE_LOOP);
  float x_begin = X(RAD(deg_angle), seg_begin);
  float y_begin = Y(RAD(deg_angle), seg_begin);
  float x_end = X(RAD(deg_angle), seg_end);
  float y_end = Y(RAD(deg_angle), seg_end);

  glVertex2f(x_begin, y_begin);
  glVertex2f(x_end, y_end);
  glEnd();
}


void PrintTextXYPos(std::string str, float x, float y) {
  size_t sz = str.size();
  glRasterPos2f(x, y);  // position to print
  for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
  }
}

//
void PrintTextRadialPos(const std::string& str, 
                     DegType deg_angle, float pos) {
  size_t sz = str.size();
  float x = X(RAD(deg_angle), pos);
  float y = Y(RAD(deg_angle), pos);
  glRasterPos2f(x, y);  // position to print
  for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
  }
}

void DrawReferenceGrid_2B_deleted() {
  // Temporary grid for reference
  // This functionis to be deleted.
  int n = 0;
  char c = n + '0';
  for (float y = -1.0; y < 1.0; y += 0.05) {
    for (float x = -1.0; x < 1.0; x += 0.05) {
      PrintTextXYPos(std::string{ c }, x, y);
      n = ++n % 10;
      c = n + '0';
    }
  }
}

void Render(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  // anti-clockwise angle
  const DegType dial_start = 315;
  const DegType dial_end = 225;
  const float inner_dial_line = 0.75;
  const float outer_dial_line = 0.8;
  //<1 Draw RPM guage range dial.
  // draw the dials inner and outer lines.
  DrawArc(dial_start, dial_end, outer_dial_line);
  DrawArc(dial_start, dial_end, inner_dial_line);
   
  // Draw RPM guage needle.
  DrawRadialSegment(135, 0.02, 0.7);
  DrawArc(0, 360, 0.02);
  PrintTextXYPos("RPM x1000", -0.15, -0.1);

  //<1.1 Mark the dial with major slots
  DegType dial_full_angle{}; // full span of the dial as an angle(in degrees).
  if (dial_start <= dial_end)
    dial_full_angle = dial_end - dial_start;
  else
    dial_full_angle = (360 - dial_start) + dial_end;

  const DegType dial_slots = 10;
  const DegType slot_angle = dial_full_angle / dial_slots;
  int slot_num = 10;
  for ( DegType mark_pos = dial_start, done_angle = 0; 
        done_angle <= dial_full_angle; // <--  not using mark_pos because it wraps over 360 deg.
        done_angle += slot_angle, mark_pos += slot_angle) {
    if (mark_pos >= 360) {
      mark_pos = mark_pos - 360;
    }
    DrawRadialSegment(mark_pos, inner_dial_line, outer_dial_line);
    
    // Mark the dial with minor slots.
    if (slot_num > 0) {
      DegType minor_slot_angle = slot_angle / 10;
      for (int i = 1; i < 10; ++i) {
        DrawRadialSegment(mark_pos + (i * minor_slot_angle), inner_dial_line, outer_dial_line - 0.03);
      }
    }
    
    // Print dial number markings.
    float offset = 0.09;
    if (mark_pos <= 135 || mark_pos > 270)
      offset = 0.05;
    PrintTextRadialPos(std::to_string(slot_num--), mark_pos, outer_dial_line + offset);
  }
  //>1.1
  //>1

  glFlush();
}

// Main loop taken from the Visual Studio setup test.
// https://www.badprog.com/c-opengl-setting-up-visual-studio
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);                    // window size
  glutInitWindowPosition(500, 500);                // distance from the top-left screen
  glutCreateWindow("RPM");    // message displayed on top bar window
  glutDisplayFunc(Render);
  glutMainLoop();
  return 0;
}