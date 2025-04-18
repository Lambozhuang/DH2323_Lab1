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
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
SDL2Aux *sdlAux;

vec3 topLeft(1, 0, 0);     // red
vec3 topRight(0, 0, 1);    // blue
vec3 bottomLeft(1, 1, 0);  // green
vec3 bottomRight(0, 1, 0); // yellow

// Starfield
vector<vec3> stars(1000);
vector<vec3> endStars(1000);

int t;
int trailLengthNow = 0;
const int trailLengthMax = 100;

// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void draw();
void interpolate(float a, float b, vector<float> &result);
void interpolate(vec3 a, vec3 b, vector<vec3> &result);
void interpolate_test_1();
void interpolate_test_2();
void update();
void task2();
void task3();
// ---------------------------------------------------------

// FUNCTION DEFINITIONS
int main(int argc, char *argv[])
{
  // Initialize stars
  for (int i = 0; i < stars.size(); ++i)
  {
    stars[i].x = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
    stars[i].y = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
    stars[i].z = float(rand()) / float(RAND_MAX);
  }
  // copy stars to endStars
  endStars = stars;

  sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);

  for (int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    for (int x = 0; x < SCREEN_WIDTH; ++x)
    {
      vec3 color(0.0, 0.0, 0.0);
      sdlAux->putPixel(x, y, color);
    }
  }

  t = SDL_GetTicks();

  while (!sdlAux->quitEvent())
  {
    update();
    draw();
  }
  sdlAux->saveBMP("screenshot.bmp");
  // interpolate_test_1();
  // interpolate_test_2();
  return 0;
}

void draw()
{
  // sdlAux->clearPixels();

  // Lab1 task2

  // task2();

  // Lab1 task3

  task3();

  sdlAux->render();
}

void update()
{
  trailLengthNow++;

  // move the stars
  int t2 = SDL_GetTicks();
  float dt = float(t2 - t);
  t = t2;
  for (int i = 0; i < stars.size(); i++)
  {
    if (stars[i].z <= 0)
    {
      stars[i].z += 1;
    }
    stars[i].z = stars[i].z - 0.0001 * dt; 
  }

  // move the trails
  if (trailLengthNow > trailLengthMax)
  {
    for (int i = 0; i < endStars.size(); i++)
    {
      if (endStars[i].z <= 0)
      {
        endStars[i].z += 1;
      }
      endStars[i].z = endStars[i].z - 0.0001 * dt;
    } 
  }
}

void task2()
{
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
}

void task3()
{
  float f = SCREEN_HEIGHT / 2;

  for (size_t i = 0; i < stars.size(); i++)
  {
    // calculate u for this star from f
    float u = f * stars[i].x / stars[i].z + SCREEN_WIDTH / 2;
    // calculate v for this star from f
    float v = f * stars[i].y / stars[i].z + SCREEN_HEIGHT / 2;
    // Generate a random color based on star position and time
    vec3 color = 0.2f * vec3(
      0.5f + 0.5f * sin(stars[i].x * 5.0f + t * 0.001f),
      0.5f + 0.5f * sin(stars[i].y * 5.0f + t * 0.002f),
      0.5f + 0.5f * sin(stars[i].z * 5.0f + t * 0.003f)
    ) / (stars[i].z * stars[i].z);

    sdlAux->putPixel(u, v, color);
    if (trailLengthNow > trailLengthMax)
    {
      // calculate u for this star from f
      float u2 = f * endStars[i].x / endStars[i].z + SCREEN_WIDTH / 2;
      // calculate v for this star from f
      float v2 = f * endStars[i].y / endStars[i].z + SCREEN_HEIGHT / 2;
      vec3 trailColor(0.0, 0.0, 0.0);
      sdlAux->putPixel(u2, v2, trailColor);
      // also draw adjacent pixels
      for (int j = -3; j <= 3; j++)
      {
        for (int k = -3; k <= 3; k++)
        {
          sdlAux->putPixel(u2 + j, v2 + k, trailColor);
        }
      }
    }
  }
}

void interpolate(float a, float b, vector<float> &result)
{
  int steps = result.size() - 1;
  if (steps == 0)
    return;
  float step = (b - a) / steps;
  for (int i = 0; i < steps; ++i)
  {
    result[i] = a + step * i;
  }
}

void interpolate(vec3 a, vec3 b, vector<vec3> &result)
{
  int steps = result.size() - 1;
  if (steps == 0)
    return;
  vec3 step = (b - a) / (float)steps;
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