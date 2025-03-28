#pragma once
#include "Common.h"
#include "ThrottleWidget.h"
#include "Dial.h"

class App {
public:
  void InitGL(int argc, char** argv);
  void Render();
  void OnMouse(int button, int state, int x, int y);

private:
  Dial _dial;
  ThrottleWidget _throttle;
};