#pragma once
#include <tuple>
#include "Common.h"

class ThrottleWidget {
public:
  void Init(unsigned maxrpm, unsigned rpm_increments) {  
    _max_rpm = maxrpm;  
    _smallest_step = rpm_increments;
  }
  void Draw();
  void OnMouse(float x, float y); // updates _rpm
  unsigned GetRPM() { return _rpm;  };

private:
  void UpdateWidgetRPMLabel();

private:
  unsigned _rpm{};
  unsigned _max_rpm{};
  unsigned _rpm_increments{};
  
  unsigned _smallest_step{};

  // Positions of the rpm numeric text box.
  float _left{ -0.85f };
  float _right{ -0.5f };
  float _top{ -0.8f };
  float _bottom{ -0.9f };


  // These buttom positions will be used for mouse hit test.
  float _btn_top{};
  float _btn_bottom{};
  float _btn_left{};
  float _btn_right{};
  float _btn_mid{};

};
