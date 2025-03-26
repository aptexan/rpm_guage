#include "Common.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <array>

// Draw an arc given:
// 1) start to end angle (anti-clockwise) in degrees and 2) the radius
//   0 (and 360) degree is on positive x-axis
//  90 on positive y-axis
// 180 on negative x-axis
// 270 on negative y-axis
void DrawArc(DegType deg_begin, DegType deg_end, float radius) {
  unsigned deg_steps{};
  if (deg_begin < deg_end) {
    deg_steps = static_cast<unsigned>(deg_end - deg_begin + 1);
  }
  else {
    deg_steps = static_cast<unsigned>((360 - deg_begin) + deg_end + 1);
  }

  glBegin(GL_LINE_STRIP);
  for (DegType deg = deg_begin; deg_steps > 0; ++deg, --deg_steps) {
    deg = deg > 360 ? 1 : deg;
    float x = RADIAL_X(RAD(deg), radius);
    float y = RADIAL_Y(RAD(deg), radius);
    glVertex2f(x, y);
  }
  glEnd();
}

//< Tessalation callbacks
void CbTessBegin() {
  glBegin(GL_POLYGON);
  std::cout << "CbTessBegin()" << std::endl;
}

void CbTessVertex(GLdouble *vert) {
  // WARNING: lose of precision(double to float)
  glVertex2d(vert[0], vert[1]);
  std::cout << (float)vert[0] << " " << (float)vert[0] << std::endl;
}

void CbTessEnd() {
  glEnd();
  std::cout << "CbTessEnd()" << std::endl;
}
//>

// Filled arc has two arcs (inner and outer) joined at the end
// with radial segment and has a fill color.
void DrawArcFilled( DegType deg_begin, DegType deg_end, 
                    float outer_radius, float inner_radius,
                    float red, float green, float blue) {

  using Vertex = std::array<GLdouble, 3>;
  // We need to pre-allocate reserve vector memory for max items
  // because, vector expansion will reallocate and move the data.
  // All this because the vertex item address needs to be the same 
  // in tesselator callbacks.
  // !!! WARNING !!!  !!! IMPORTANT !!!
  // Big assumption here that the granularity of stepping through
  // the angle is minimum 1 degree.
  std::vector<Vertex> verts(360 * 2 + 4, Vertex{});
  size_t vert_idx{}; // vertices index
  
  unsigned deg_steps{};
  if (deg_begin < deg_end) {
    deg_steps = static_cast<unsigned>(deg_end - deg_begin + 1);
  }
  else {
    deg_steps = static_cast<unsigned>((360 - deg_begin) + deg_end + 1);
  }
  
  //Setup tesselator
  GLUtesselatorObj* tess = gluNewTess();
  gluTessCallback(tess, GLU_TESS_BEGIN, CbTessBegin);
  gluTessCallback(tess, GLU_TESS_VERTEX, (void(GLAPIENTRY*)(void))CbTessVertex);
  gluTessCallback(tess, GLU_TESS_END, CbTessEnd);
  gluTessBeginPolygon(tess, NULL);
  gluTessBeginContour(tess);
  

  unsigned deg_steps_saved = deg_steps;
  // deg :  This var will be the angle adjusted as we draw 
  //        outer arc, left radial segment, inner arc 
  //        and right radial segment, completing the poly.
  DegType deg = deg_begin; 
  glColor3f(red, blue, green);
  // vertices for outer arc - anti-clockwise
  for (; deg_steps > 0; ++deg, --deg_steps) {
    deg = deg > 360 ? 1 : deg;
    verts[vert_idx][0] = RADIAL_X(RAD(deg), outer_radius);
    verts[vert_idx][1] = RADIAL_Y(RAD(deg), outer_radius);
    gluTessVertex(tess, (GLdouble*)(verts[vert_idx].data()), (GLdouble*)(verts[vert_idx].data()));
    ++vert_idx;
  }

  // left most inner arc vertex.
  verts[vert_idx][0] = RADIAL_X(RAD(deg), inner_radius);
  verts[vert_idx][1] = RADIAL_Y(RAD(deg), inner_radius);
  gluTessVertex(tess, (GLdouble*)(verts[vert_idx].data()), (GLdouble*)(verts[vert_idx].data()));
  ++vert_idx;

  // vertices for inner arc - clockwise
  deg_steps = deg_steps_saved;
  for (; deg_steps > 0; --deg, --deg_steps) {
    deg = deg < 360 ? 0 : deg;
    verts[vert_idx][0] = RADIAL_X(RAD(deg), inner_radius);
    verts[vert_idx][0] = RADIAL_Y(RAD(deg), inner_radius);
    gluTessVertex(tess, (GLdouble*)(verts[vert_idx].data()), (GLdouble*)(verts[vert_idx].data()));
    ++vert_idx;
  }

  // right most outer arc vertex.
  // May be not needed as tesselator makes a loop ending with the first vertex.
  verts[vert_idx][0] = RADIAL_X(RAD(deg), outer_radius);
  verts[vert_idx][0] = RADIAL_Y(RAD(deg), outer_radius);
  gluTessVertex(tess, (GLdouble*)(verts[vert_idx].data()), (GLdouble*)(verts[vert_idx].data()));


  gluTessEndContour(tess);
  gluTessEndPolygon(tess);
  gluDeleteTess(tess);
}


// Draw a line segment from a center/origin radiating outwards.
// give:
// deg_angle: the direction 
// seg_begin: starting distance from center
// seg_end  : ending distance from the center
// As an example, a needle for RPM gauge can be drawn as a radial segment.
void DrawRadialSegment(DegType deg_angle, float seg_begin, float seg_end) {
  glBegin(GL_LINE_LOOP);
  float x_begin = RADIAL_X(RAD(deg_angle), seg_begin);
  float y_begin = RADIAL_Y(RAD(deg_angle), seg_begin);
  float x_end = RADIAL_X(RAD(deg_angle), seg_end);
  float y_end = RADIAL_Y(RAD(deg_angle), seg_end);

  glVertex2f(x_begin, y_begin);
  glVertex2f(x_end, y_end);
  glEnd();
}

void PrintTextXYPos(std::string str, float x, float y) {
  size_t sz = str.size();
  glRasterPos2f(x, y);  // position to print
  for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
  }
}

void PrintTextRadialPos(const std::string& str,
  DegType deg_angle, float pos) {
  size_t sz = str.size();
  float x = RADIAL_X(RAD(deg_angle), pos);
  float y = RADIAL_Y(RAD(deg_angle), pos);
  glRasterPos2f(x, y);  // position to print
  for (size_t i = 0; i < sz; i++) { // loop until i is greater then l
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); // Print a character on the screen
  }
}

float NormalizeX(int x) {
  return (((float)x / WIN_WD) * 2) - 1;
}

float NormalizeY(int y) {
  return -1 * ((((float)y / WIN_HT) * 2) - 1);
}
