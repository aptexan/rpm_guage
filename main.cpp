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

#include "Common.h"
#include "App.h"

// !!! RESTRICTION: !!!
// 1) Window size will be always(hardcoded) to 500 x 500
// 2) Resize will not be honored. Attempting to resize will skew the display 
//    The interaction with the window and its content will be undefined.


App g_app;


//< C-style Callback signature required in App::InitGL()
void OnMouse(int button, int state, int x, int y) {
  g_app.OnMouse(button, state, x, y);
}

void Render(void) {
  g_app.Render();
}
//>

int main(int argc, char** argv) {
  g_app.InitGL(argc, argv);
  return 0;
}
