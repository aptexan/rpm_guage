#include "ThrottleWidget.h"
#include "Common.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

  void ThrottleWidget::OnMouse(float x, float y) {
    if (_btn_top > y && y > _btn_mid
      && _btn_right > x && x > _btn_left) {
      // UP button hit.
      _rpm = _rpm >= _max_rpm ? _max_rpm : _rpm + _smallest_step;
    }
    if (_btn_mid > y && y > _btn_bottom
      && _btn_right > x && x > _btn_left) {
      // DOWN button hit.
      _rpm = _rpm <= 0 ? 0 : _rpm - _smallest_step;
    }
  }

  void ThrottleWidget::Draw() {
    // RPM numeric display - clock-wise vertices
    glBegin(GL_LINE_LOOP);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();

    //< Throttle high/low buttons - clockwise vertices
    glBegin(GL_LINE_LOOP);
    _btn_top = top + 0.05f;
    _btn_bottom = bottom - 0.05f;
    _btn_left = right;
    _btn_right = right + 0.1f;
    glVertex2f(_btn_left, _btn_top);
    glVertex2f(_btn_right, _btn_top);
    glVertex2f(_btn_right, _btn_bottom);
    glVertex2f(_btn_left, _btn_bottom);
    glEnd();
    glBegin(GL_LINE_STRIP);
    _btn_mid = _btn_top + (_btn_bottom - _btn_top) / 2;
    glVertex2f(_btn_left, _btn_mid);
    glVertex2f(_btn_right, _btn_mid);
    glEnd();
    // Up and down triangle in the throttle buttons  
    float btn_width = fabs(_btn_left - _btn_right);
    float margin = 0.01f;
    glBegin(GL_TRIANGLES);
    glColor3f(0, 0.5f, 0);
    glVertex2f((_btn_left + btn_width / 2), _btn_top - margin); // x, y of up botton top vertex.
    glVertex2f(_btn_right - margin, _btn_mid + margin);         // x, y of next clock wise vertices, here on.
    glVertex2f(_btn_left + margin, _btn_mid + margin);
    glEnd();
    // same for down btn 
    glBegin(GL_TRIANGLES);
    glColor3f(0.5, 0, 0);
    glVertex2f((_btn_left + btn_width / 2), _btn_bottom + margin); // x, y of down botton bottom vertex.
    glVertex2f(_btn_left + margin, _btn_mid - margin);             // x, y of next clock wise vertices, here on.
    glVertex2f(_btn_right - margin, _btn_mid - margin);
    glColor3f(1, 1, 1);
    glEnd();
  }
