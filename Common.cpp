#include "Common.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <array>

//< Tessellation callback functions
void GLAPIENTRY CbTessBegin(GLenum type) {
  glBegin(type);
  //printf("Tess Begin: %d\n", type);
}

void GLAPIENTRY CbTessVertex(void* vertex_data) {
  GLdouble* vert = (GLdouble*)vertex_data;
  glVertex3dv(vert);
  //printf("Tess Vertex: %f, %f, %f\n", vert[0], vert[1], vert[2]);
}

void GLAPIENTRY CbTessEnd() {
  glEnd();
  //printf("Tess End\n");
}

void GLAPIENTRY CbTessCombine(GLdouble coords[3], void* vertex_data[4],
  GLfloat weight[4], void** outData) {
  GLdouble* vertex = new GLdouble[3];
  vertex[0] = coords[0];
  vertex[1] = coords[1];
  vertex[2] = coords[2];
  *outData = vertex;
  //printf("Tess Combine: %f, %f, %f\n", coords[0], coords[1], coords[2]);
}
//>

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

// Filled arc has two arcs (inner and outer) joined at the end
// with radial segment and has a fill color.
void DrawArcFilled( const DegType deg_begin, const DegType deg_end, 
                    const float outer_radius, const float inner_radius,
                    const float red, const float green, const float blue) {
  float step_size = 0.25;
  float deg_steps{};
  if (deg_begin < deg_end) {
    deg_steps = static_cast<unsigned>(deg_end - deg_begin + 1);
  }
  else {
    deg_steps = static_cast<unsigned>((360 - deg_begin) + deg_end + 1);
  }
  deg_steps /= step_size;

  using Vertex = std::array<GLdouble, 3>;
  // We need to pre-allocate reserve vector memory for max items
  // because, vector expansion will reallocate and move the data.
  // All this because the vertex item address needs to be the same 
  // in tesselator callbacks.
  // !!! WARNING !!!
  // Assumption here that the granularity of stepping through
  // the angle is minimum 1 degree.
  std::vector<Vertex> verts((360 * 2 + 4)/step_size, Vertex{});
  size_t vert_idx{}; // vertices index
  
  
  //Setup tesselator
  GLUtesselator* tess = gluNewTess();
  if (!tess) {
    printf("Failed to create tessellator\n");
    return;
  }

  // Set callbacks
  gluTessCallback(tess, GLU_TESS_BEGIN, (void (GLAPIENTRY*)())CbTessBegin);
  gluTessCallback(tess, GLU_TESS_VERTEX, (void (GLAPIENTRY*)())CbTessVertex);
  gluTessCallback(tess, GLU_TESS_END, (void (GLAPIENTRY*)())CbTessEnd);
  gluTessCallback(tess, GLU_TESS_COMBINE, (void (GLAPIENTRY*)())CbTessCombine);

  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

  glColor3f(red, green, blue);
  gluTessBeginPolygon(tess, nullptr);
  gluTessBeginContour(tess);

  float deg_steps_saved = deg_steps;
  // deg :  This var will be the angle adjusted as we draw 
  //        outer arc, left radial segment, inner arc 
  //        and right radial segment, completing the poly.
  DegType deg = deg_begin; 
  // vertices for outer arc - anti-clockwise
  for (; deg_steps > 0; deg = deg + step_size, --deg_steps) {
    verts[vert_idx][0] = RADIAL_X(RAD(deg), outer_radius);
    verts[vert_idx][1] = RADIAL_Y(RAD(deg), outer_radius);
    //printf("Outer: %f, %f\n", verts[vert_idx][0], verts[vert_idx][1]);
    gluTessVertex(tess, verts[vert_idx].data(), verts[vert_idx].data());
    ++vert_idx;
  }

  // vertices for inner arc - clockwise
  deg_steps = deg_steps_saved;
  for (; deg_steps > 0; deg = deg - step_size, --deg_steps) {
    verts[vert_idx][0] = RADIAL_X(RAD(deg), inner_radius);
    verts[vert_idx][1] = RADIAL_Y(RAD(deg), inner_radius);
    //printf("Inner: %f, %f\n", verts[vert_idx][0], verts[vert_idx][1]);
    gluTessVertex(tess, verts[vert_idx].data(), verts[vert_idx].data());
    ++vert_idx;
  }

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
