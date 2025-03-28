#pragma once

#include "Dial.h"
#include "ThrottleWidget.h"

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <cstdio>
#include <vector>

bool Dial::Init(DegType start_angle, DegType end_angle, float outer_radius, float inner_radius,
  unsigned max_rpm, unsigned rpm_per_slot,
  unsigned rpm_green, unsigned rpm_yellow, unsigned rpm_red, ThrottleWidget* throttle) {
  if (!throttle) {
    printf("ERROR: Must provide a pointer to ThrottleWidget object\n");
    return false;
  }

  _throttle = throttle;
  dial_start = start_angle;
  dial_end = end_angle;
  outer_dial_radius = outer_radius;
  inner_dial_radius = inner_radius;
  _max_rpm = max_rpm;
  _rpm_per_slot = rpm_per_slot;
  _rpm_green_start = rpm_green;
  _rpm_yellow_start = rpm_yellow;
  _rpm_red_start = rpm_red;

  if (dial_start <= dial_end)
    dial_full_angle = dial_end - dial_start;
  else
    dial_full_angle = (360 - dial_start) + dial_end;

  // Calculate red zone start angle
  DegType  angle_per_rpm = dial_full_angle / _max_rpm;

  DegType red_start{ dial_start }, red_end{};
  printf("angle_per_rpm: %f\n", angle_per_rpm);
  red_end = red_start + ((_max_rpm - _rpm_red_start) * angle_per_rpm);
  printf("( ((_max_rpm - _rpm_red_start) * angle_per_rpm: %f\n", ((_max_rpm - _rpm_red_start) * angle_per_rpm));
  printf("red_end: %f\n", red_end);
  if (red_end > 360)
    red_end = (red_end - 360);
  _rpm_red_angle = red_end;

  // Calculate yellow zone start angle
  DegType yellow_start{ red_end }, yellow_end{};
  printf("angle_per_rpm: %f\n", angle_per_rpm);
  yellow_end = yellow_start + ((_rpm_red_start - _rpm_yellow_start) * angle_per_rpm);
  printf("(_max_rpm - _rpm_red_start) * angle_per_rpm: %f\n", ((_max_rpm - _rpm_red_start) * angle_per_rpm));
  printf("yellow_end: %f\n", yellow_end);
  if (yellow_end > 360)
    yellow_end = (yellow_end - 360);
  _rpm_yellow_angle = yellow_end;

  // Calculate green zone start angle
  DegType green_start{ yellow_end }, green_end{};
  printf("angle_per_rpm: %f\n", angle_per_rpm);
  green_end = green_start + ((_rpm_yellow_start - _rpm_green_start) * angle_per_rpm);
  printf("( ((_max_rpm - _rpm_red_start) * angle_per_rpm: %f\n", ((_max_rpm - _rpm_red_start) * angle_per_rpm));
  printf("green_end: %f\n", green_end);
  if (green_end > 360)
    green_end = (green_end - 360);
  _rpm_green_angle = green_end;
  return true;
}

void Dial::Draw() {
  //< Draw the dial Arc
  // Fill the dial with RPM colored range zones 
  DrawArcFilled(dial_start, dial_end, outer_dial_radius, inner_dial_radius,
    0.3f, 0.3f, 0.3f); // gray background
  DrawArcFilled(dial_start, _rpm_red_angle, outer_dial_radius, inner_dial_radius,
    0.5f, 0.0f, 0.0f); // red zone
  DrawArcFilled(_rpm_red_angle, _rpm_yellow_angle, outer_dial_radius, inner_dial_radius,
    5.0f, 0.5f, 0.0f); // yellow zone
  DrawArcFilled(_rpm_yellow_angle, _rpm_green_angle, outer_dial_radius, inner_dial_radius,
    0.0f, 0.5f, 0.0f); // green zone
  // Rest is gray. 
  glColor3f(1, 1, 1);
  // Draw the dials inner and outer lines (draw borders).
  DrawArc(dial_start, dial_end, outer_dial_radius);
  DrawArc(dial_start, dial_end, inner_dial_radius);


  //< Draw needle.
  unsigned rpm = _throttle->GetRPM();
  DegType ndl_rpm_angle = dial_start + (dial_full_angle * (_max_rpm - rpm)) / _max_rpm;
  float ndl_w = 0.05f;     // needle width
  float ndl_l = inner_dial_radius;
  float ndl_tip_l = 0.05f;  // needle tip length
  glColor3f(1, 1, 1);
  DrawArcFilled(0, 360, ndl_w / 2, 0.001f, 1, 1, 1); // filled circle at needle pivot point.
  glRotatef(ndl_rpm_angle, 0, 0, 1);
  glBegin(GL_POLYGON);
  std::vector<Point> ndl_verts{  // needle vertices
    { 0.0f, ndl_w / 2.0f },
    { ndl_l - ndl_tip_l, ndl_w / 2.0f},
    { ndl_l, 0.0f},
    { ndl_l - ndl_tip_l, -ndl_w / 2.0f},
    { 0.0f, -ndl_w / 2.0f}
  };
  for (auto& v : ndl_verts) {
    glVertex2f(v.x, v.y);
  }
  glEnd();
  glRotatef(-1 * ndl_rpm_angle, 0, 0, 1); // Undo the rotation for rest of the drawing.
  //> End draw needle 

  PrintTextXYPos("RPM x1000", -0.2f, -0.3f);

  // Mark the dial with major slots
  const DegType dial_slots = _max_rpm / _rpm_per_slot;
  const DegType slot_angle = dial_full_angle / dial_slots;
  int slot_num = dial_slots;
  for (DegType mark_pos = dial_start, done_angle = 0;
    done_angle <= dial_full_angle; // <--  not using mark_pos because it wraps over 360 deg.
    done_angle += slot_angle, mark_pos += slot_angle) {
    if (mark_pos >= 360) {
      mark_pos = mark_pos - 360;
    }
    DrawRadialSegment(mark_pos, inner_dial_radius, outer_dial_radius);

    // Mark the dial with minor slots.
    // Minor slot markings are always 10
    if (slot_num > 0) {
      DegType minor_slot_angle = slot_angle / 10;
      for (int i = 1; i < 10; ++i) {
        DrawRadialSegment(mark_pos + (i * minor_slot_angle), inner_dial_radius, outer_dial_radius - 0.03f);
      }
    }

    // Print then dial's major slot numbers.
    float offset = 0.09f;
    if (mark_pos <= 135 || mark_pos > 270)
      offset = 0.05f;
    PrintTextRadialPos(std::to_string(slot_num--), mark_pos, outer_dial_radius + offset);
  } // outer for
}
