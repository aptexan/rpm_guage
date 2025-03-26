#pragma once

class ThrottleWidget {
public:
  void Draw();
  void OnMouse(float x, float y);
  unsigned GetRPM() { return _rpm;  };

private:
  unsigned _rpm{};

  // These positions will be used for mouse hit test.
  float _btn_top{};
  float _btn_bottom{};
  float _btn_left{};
  float _btn_right{};
  float _btn_mid{};
};
