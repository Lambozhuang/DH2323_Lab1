//from https://github.com/lemonad/DH2323-Skeleton
#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include "SDL2Auxiliary.h"

using namespace std;

/*
 * Construct a window with an associated pixel buffer to
 * draw into.
 *
 * If the fullscreen flag is set, the window is maximized
 * to the desktop size (but resolution is not changed).
 * In this case, width and height is only used for the
 * size of the pixel buffer.
 */
SDL2Aux::SDL2Aux(int width, int height, bool fullscreen) {
  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;

  if (!initializeSDL() ||
      !createWindow() ||
      !createRenderer() ||
      !createTexture() ||
      !createPixelBuffer()) {
    cout << "Could not initialize SDLAux. Exiting." << endl;
    exit(1);
  }

  atexit(SDL_Quit);
}


/*
 * Free allocated pixel buffer. We don't have to destroy SDL
 * objects here because that is taken care of by SDL_Quit.
 */
SDL2Aux::~SDL2Aux() {
  if (pixel_buffer != NULL) {
    free(pixel_buffer);
  }
}


/*
 * Sets up SDL (video and timer) for per pixel drawing.
 *
 * Returns true on success.
 */
bool SDL2Aux::initializeSDL() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    cout << "Could not initialize SDL: " << SDL_GetError() << endl;
    return false;
  }

  return true;
}


/*
 * Creates a window for rendering to.
 *
 * Returns true on success.
 */
bool SDL2Aux::createWindow() {
  int flags = SDL_WINDOW_ALLOW_HIGHDPI;
  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  sdl_window = SDL_CreateWindow("SDL",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                width,
                                height,
                                flags);
  if (sdl_window == NULL) {
    cout << "Could not create SDL window: " << SDL_GetError() << endl;
    return false;
  }

  return true;
}


/*
 * Creates a renderer which is scaled up so we don't get
 * a stamp-sized output on screen.
 *
 * Returns true on success.
 */
bool SDL2Aux::createRenderer() {
  // Just pick any renderer available (providing no flags gives
  // priority to available hardware accelerated renderers.)
  sdl_renderer = SDL_CreateRenderer(sdl_window,
                                    -1,
                                    SDL_RENDERER_PRESENTVSYNC);
  if (sdl_renderer == NULL) {
    cout << "Could not create SDL renderer: " << SDL_GetError() << endl;
    return false;
  }

  // Make the scaled rendering look smoother.
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(sdl_renderer, width, height);
  return true;
}


/*
 * Create the texture which represents the whole screen. The pixel
 * buffer is used to update this texture which is then rendered to
 * screen.
 *
 * Returns true on success.
 */
bool SDL2Aux::createTexture() {
  // This represents a texture on the GPU.
  // SDL_TEXTUREACCESS_STREAMING tells SDL that this texture's
  // contents is going to change frequently.
  sdl_texture = SDL_CreateTexture(sdl_renderer,
                                  SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  width,
                                  height);
  if (sdl_texture == NULL) {
    cout << "Could not create SDL texture: " << SDL_GetError() << endl;
    return false;
  }

  return true;
}


/*
 * Create the pixel buffer for per-pixel drawing into.
 *
 * Returns true on success.
 */
bool SDL2Aux::createPixelBuffer() {
  pixel_buffer = (Uint32 *)calloc(width * height, sizeof(Uint32));
  if (pixel_buffer == NULL) {
    cout << "Could not create SDL pixel buffer." << endl;
    return false;
  }

  return true;
}


/*
 * Clears the pixel buffer (i.e. sets it to black).
 */
void SDL2Aux::clearPixels() {
  memset(pixel_buffer, 0, width * height * sizeof(Uint32));
}


/*
 * Update a pixel in the pixel buffer. The color is represented
 * by a glm:vec3 which specifies the red, green and blue components
 * with numbers between 0.0 and 1.0 (inclusive).
 */
void SDL2Aux::putPixel(int x, int y, glm::vec3 color) {
  if (x < 0 || x >= width ||
      y < 0 || y >= height) {
    return;
  }

  Uint8 red = Uint8(glm::clamp(255 * color.r, 0.f, 255.f));
  Uint8 green = Uint8(glm::clamp(255 * color.g, 0.f, 255.f));
  Uint8 blue = Uint8(glm::clamp(255 * color.b, 0.f, 255.f));
  Uint8 alpha = 255;

  // Each pixel in the pixel buffer consists of four bytes: 0xAARRGGBB.
  Uint32 rgba = (alpha << 24) + (red << 16) + (green << 8) + blue;

  // Calculate the address of the pixel we want to set.
  Uint32* pixel = (Uint32 *)pixel_buffer + y * width + x;

  // TODO big endian support?
  // #if SDL_BYTEORDER == SDL_BIG_ENDIAN
  // #else
  // #endif

  *pixel = rgba;
}


/*
 * Use the pixel buffer to update the texture, then render the
 * texture into the window/screen.
 */
void SDL2Aux::render() {
  SDL_UpdateTexture(sdl_texture,
                    NULL,
                    pixel_buffer,
                    width * sizeof(Uint32));

  SDL_RenderClear(sdl_renderer);
  SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
  SDL_RenderPresent(sdl_renderer);
}


/*
 * Save pixel buffer as a bitmap file.
 *
 * Returns true on success.
 */
bool SDL2Aux::saveBMP(const char *filename) {
  // TODO big endian support?
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(pixel_buffer,
                                                  width,
                                                  height,
                                                  32,
                                                  width * 4,
                                                  0, 0, 0, 0);
  if (surface == NULL) {
    cout << "Could not create SDL surface for bitmap: "
         << SDL_GetError() << endl;
    return false;
  }

  SDL_SaveBMP(surface, filename);
  SDL_FreeSurface(surface);
  return true;
}


/*
 * Goes through the SDL event queue looking for events corresponding
 * to the user wanting to quit/exit.
 *
 * Returns true if a quit event was received.
 */
bool SDL2Aux::quitEvent() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return true;
    }

    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        return true;
      }
    }
  }

  return false;
}


/*
 * Updates the window title.
 */
void SDL2Aux::setWindowTitle(const char *title) {
  SDL_SetWindowTitle(sdl_window, title);
}