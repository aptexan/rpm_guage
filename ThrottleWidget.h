#pragma once

class ThrottleWidget {
public:
  void Draw();
  void OnMouse(float x, float y); // updates _rpm
  unsigned GetRPM() { return _rpm;  };
  unsigned GetMaxRPM() { return _max_rpm;  }
private:
  void UpdateWidgetRPMLabel();

private:
  unsigned _rpm{};
  const unsigned _max_rpm{ 10000 }; // Widget will set _rpm above _max_rpm value.
  const unsigned _smallest_step = 50;

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
