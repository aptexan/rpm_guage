#pragma once

#include "Common.h"

class ThrottleWidget;

class Dial {
public:
  bool Init(DegType start_angle, DegType end_angle, float outer_radius, float inner_radius,
    unsigned max_rpm, unsigned rpm_per_slot,
    unsigned rpm_green, unsigned rpm_yellow, unsigned rpm_red, ThrottleWidget* throttle);
  void Draw();

private:
  unsigned _max_rpm{}; // Widget will not set _rpm above _max_rpm value.
  unsigned _rpm_per_slot{};
  ThrottleWidget* _throttle{};

  DegType dial_start{};
  DegType dial_end{};
  float inner_dial_radius{};
  float outer_dial_radius{};

  DegType dial_full_angle{};

  // Zones
  unsigned _rpm_red_start = 8000;
  unsigned _rpm_yellow_start = 7000;
  unsigned _rpm_green_start = 3000;

  // Calculate and cached
  float _rpm_red_angle{};
  float _rpm_yellow_angle{};
  float _rpm_green_angle{};
};