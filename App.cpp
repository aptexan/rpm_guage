#include "App.h"
#include "Types.h"
#include "ThrottleWidget.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

#include <iostream>
#include <string>


  void App::InitGL(int argc, char** argv) {
    // Main loop taken from the Visual Studio setup test.
    // https://www.badprog.com/c-opengl-setting-up-visual-studio
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_WD, WIN_HT);              // window size
    glutInitWindowPosition(500, 500);                // distance from the top-left screen
    glutCreateWindow("RPM");    // message displayed on top bar window
    glutDisplayFunc(::Render);
    glutMouseFunc(::OnMouse);
    glutMainLoop();
  }

  void App::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    DrawDial();
    _throttle.Draw();
    glFlush();
  }

  void App::OnMouse(int button, int state, int x, int y) {
    // notmalize from -1 to 1
    float x_norm = NormalizeX(x);
    float y_norm = NormalizeY(y);
    _throttle.OnMouse(x_norm, y_norm);
  }


  // Draw an arc given:
  // 1) start to end angle (anti-clockwise) in degrees and 2) the radius
  //   0 (and 360) degree is on positive x-axis
  //  90 on positive y-axis
  // 180 on negative x-axis
  // 270 on negative y-axis
  void App::DrawArc(DegType deg_begin, DegType deg_end, float radius) {
    unsigned deg_steps{};
    if (deg_begin < deg_end) {
      deg_steps = static_cast<unsigned>(deg_end - deg_begin + 1);
    }
    else {
      deg_steps = static_cast<unsigned>((360 - deg_begin) + deg_end + 1);
    }

    glBegin(GL_LINE_STRIP);
    for (DegType deg = deg_begin; deg_steps > 0; ++deg, --deg_steps) {
      deg = deg > 360 ? 1 : deg;
      float x = RADIAL_X(RAD(deg), radius);
      float y = RADIAL_Y(RAD(deg), radius);
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
  void App::DrawRadialSegment(DegType deg_angle, float seg_begin, float seg_end) {
    glBegin(GL_LINE_LOOP);
    float x_begin = RADIAL_X(RAD(deg_angle), seg_begin);
    float y_begin = RADIAL_Y(RAD(deg_angle), seg_begin);
    float x_end = RADIAL_X(RAD(deg_angle), seg_end);
    float y_end = RADIAL_Y(RAD(deg_angle), seg_end);

    glVertex2f(x_begin, y_begin);
    glVertex2f(x_end, y_end);
    glEnd();
  }

  void App::PrintTextXYPos(std::string str, float x, float y) {
    size_t sz = str.size();
    glRasterPos2f(x, y);  // position to print
    for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
    }
  }

  void App::PrintTextRadialPos(const std::string& str,
    DegType deg_angle, float pos) {
    size_t sz = str.size();
    float x = RADIAL_X(RAD(deg_angle), pos);
    float y = RADIAL_Y(RAD(deg_angle), pos);
    glRasterPos2f(x, y);  // position to print
    for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
    }
  }

  void App::DrawReferenceGrid_2B_deleted() {
    // Temporary grid for reference
    // This functionis to be deleted.
    int n = 0;
    char c = n + '0';
    for (float y = -1.0f; y < 1.0f; y += 0.05f) {
      for (float x = -1.0f; x < 1.0f; x += 0.05f) {
        PrintTextXYPos(std::string{ c }, x, y);
        n = ++n % 10;
        c = n + '0';
      }
    }
  }

  float App::NormalizeX(int x) {
    return (((float)x / WIN_WD) * 2) - 1;
  }

  float App::NormalizeY(int y) {
    return -1 * ((((float)y / WIN_HT) * 2) - 1);
  }

  void App::DrawDial() {
    // anti-clockwise angle
    const DegType dial_start = 315;
    const DegType dial_end = 225;
    const float inner_dial_line = 0.75f;
    const float outer_dial_line = 0.8f;

    // Draw the dials inner and outer lines.
    DrawArc(dial_start, dial_end, outer_dial_line);
    DrawArc(dial_start, dial_end, inner_dial_line);

    DegType dial_full_angle{}; // full span of the dial as an angle(in degrees).
    if (dial_start <= dial_end)
      dial_full_angle = dial_end - dial_start;
    else
      dial_full_angle = (360 - dial_start) + dial_end;

    // Draw RPM guage needle.
    unsigned rpm = _throttle.GetRPM();
    DegType needle_angle = dial_start + (dial_full_angle * (10000 - rpm)) / 10000;
    DrawRadialSegment(needle_angle, 0.02f, 0.7f);
    DrawArc(0, 360, 0.02f);
    PrintTextXYPos("RPM x1000", -0.15f, -0.1f);

    // Mark the dial with major slots
    const DegType dial_slots = 10;
    const DegType slot_angle = dial_full_angle / dial_slots;
    int slot_num = 10;
    for (DegType mark_pos = dial_start, done_angle = 0;
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
          DrawRadialSegment(mark_pos + (i * minor_slot_angle), inner_dial_line, outer_dial_line - 0.03f);
        }
      }

      // Print then dial's major slot numbers.
      float offset = 0.09f;
      if (mark_pos <= 135 || mark_pos > 270)
        offset = 0.05f;
      PrintTextRadialPos(std::to_string(slot_num--), mark_pos, outer_dial_line + offset);
    }
  }
