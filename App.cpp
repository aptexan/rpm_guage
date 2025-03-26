#include "App.h"
#include "Common.h"
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
