// DH2323 Lab 1
// Introduction lab that covers:
// * SDL2 (https://www.libsdl.org/)
// * C++, std::vector and glm::vec3 (https://glm.g-truc.net)
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation

#include "SDL2Auxiliary.h"
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace std;
using glm::vec3;

// ---------------------------------------------------------
// GLOBAL VARIABLES
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL2Aux *sdlAux;

vec3 topLeft(1, 0, 0);     // red
vec3 topRight(0, 0, 1);    // blue
vec3 bottomLeft(0, 1, 0);  // green
vec3 bottomRight(1, 1, 0); // yellow

// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void draw();
void interpolate(float a, float b, vector<float> &result);
void interpolate(vec3 a, vec3 b, vector<vec3> &result);
void interpolate_test_1();
void interpolate_test_2();
// ---------------------------------------------------------

// FUNCTION DEFINITIONS
int main(int argc, char *argv[])
{
  sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
  while (!sdlAux->quitEvent())
  {
    draw();
  }
  sdlAux->saveBMP("screenshot.bmp");
  // interpolate_test_1();
  // interpolate_test_2();
  return 0;
}

void draw()
{
  sdlAux->clearPixels();

  vector<vec3> left_side(SCREEN_HEIGHT);
  vector<vec3> right_side(SCREEN_HEIGHT);
  interpolate(topLeft, bottomLeft, left_side);
  interpolate(topRight, bottomRight, right_side);

  for (int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    vector<vec3> row(SCREEN_WIDTH);
    interpolate(left_side[y], right_side[y], row);
    for (int x = 0; x < SCREEN_WIDTH; ++x)
    {
      sdlAux->putPixel(x, y, row[x]);
    }
  }

  sdlAux->render();
}

void interpolate(float a, float b, vector<float> &result)
{
  int steps = result.size();
  if (steps == 0)
    return;
  float step = (b - a) / (steps - 1);
  for (int i = 0; i < steps; ++i)
  {
    result[i] = a + step * i;
  }
}

void interpolate(vec3 a, vec3 b, vector<vec3> &result)
{
  int steps = result.size();
  if (steps == 0)
    return;
  vec3 step = (b - a) / (float)(steps - 1);
  for (int i = 0; i < steps; ++i)
  {
    result[i] = a + step * (float)i;
  }
}

void interpolate_test_1()
{
  vector<float> result(10);
  interpolate(1.0, 2.0, result);
  for (int i = 0; i < result.size(); ++i)
  {
    cout << result[i] << " ";
  }
}

void interpolate_test_2()
{
  vector<vec3> result(4);
  vec3 a(1, 4, 9.2);
  vec3 b(4, 1, 9.8);
  interpolate(a, b, result);
  for (int i = 0; i < result.size(); ++i)
  {
    cout << "( "
         << result[i].x << ", "
         << result[i].y << ", "
         << result[i].z << " ) ";
  }
}