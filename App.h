#pragma once
#include "Common.h"
#include "ThrottleWidget.h"

#include <string>

class App {
public:
  void InitGL(int argc, char** argv);
  void Render();
  void OnMouse(int button, int state, int x, int y);

private:
   void DrawDial();

private:
  ThrottleWidget _throttle;
};