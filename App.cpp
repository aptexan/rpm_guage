#include "App.h"
#include "Common.h"
#include "ThrottleWidget.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <ranges>

  void App::InitGL(int argc, char** argv) {
    // Main loop taken from the Visual Studio setup test.
    // https://www.badprog.com/c-opengl-setting-up-visual-studio
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutInitWindowSize(WIN_WD, WIN_HT);              // window size
    glutInitWindowPosition(500, 500);                // distance from the top-left screen
    glutCreateWindow("RPM");    // message displayed on top bar window
    glutDisplayFunc(::Render);
    glutMouseFunc(::OnMouse);


#define CONFIG  0

#if CONFIG == 1

    //< Configure the Dial and Throttle controller/widget.
    const unsigned max_rpm{ 4000 };
    // The dial arc is divided in to slots
    // First slot start with 0 RPM, last slot ends with max_rpm.
    // each slots will always have 10 sub/minor slots.
    const unsigned rpm_per_slot{ 1000 };

    // throttle controller/widget
    _throttle.Init(max_rpm, 
                   50);      // Up/Down button RPM increment/decrement amount.
    // dial display
    _dial.Init( 0,    // degree where right most dial arc starts (thats the max rpm side).
                180,    // degree where dial left most arc ends - 0 RPO side
                0.7f,    // outer radius of dial
                0.625f,   // inner
                max_rpm,
                rpm_per_slot,
                1500, 3000, 3300, // Where green, yellow, red RPM zone starts
                &_throttle); // Widget to control throttle/RPM
    //>
    
#else 
    //< Configure the Dial and Throttle controller/widget.
    const unsigned max_rpm{ 10'000 };
    // The dial arc is divided in to slots
    // First slot start with 0 RPM, last slot ends with max_rpm.
    // each slots will always have 10 sub/minor slots.
    const unsigned rpm_per_slot{ 1000 };

    // throttle controller/widget
    _throttle.Init(max_rpm,
      100);      // Up/Down button RPM increment/decrement amount.
    // dial display
    _dial.Init(315,    // degree where right most dial arc starts (thats the max rpm side).
      225,    // degree where dial left most arc ends - 0 RPO side
      0.8f,    // outer radius of dial
      0.75f,   // inner
      max_rpm,
      rpm_per_slot,
      2000, 7000, 8000,   // Where green, yellow, red RPM zone starts
      &_throttle); // Widget to control throttle/RPM
    //>

#endif

    glutMainLoop();
  }

  void App::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    _dial.Draw();
    _throttle.Draw();
    glFlush();
  }

  void App::OnMouse(int button, int state, int x, int y) {
    // notmalize from -1 to 1
    if (state == 1) {
      float x_norm = NormalizeX(x);
      float y_norm = NormalizeY(y);
      _throttle.OnMouse(x_norm, y_norm);
    }
  }

