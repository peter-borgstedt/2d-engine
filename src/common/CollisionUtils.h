#ifndef COLLISION_UTILS_H
#define COLLISION_UTILS_H

#define COLLIDE_LEFT   0x00000001U
#define COLLIDE_RIGHT  0x00000002U
#define COLLIDE_TOP    0x00000004U
#define COLLIDE_BOTTOM 0x00000008U

#include <SDL.h> // Needed for pixel collision.
#include <algorithm>

namespace collision_utils
{
  // An anonymous namespace, encapsulate functions within this scope. These are
  // help functions and intended for internal usage.
  namespace
  {
    // Retrieve pixels depending on format.
    // Ref: http://sdl.beuc.net/sdl.wiki/Pixel_Access
    inline Uint32 get_pixel(void *pixels, int bpp, int pitch, int x, int y)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      if (bpp == 1) { // Uint8
        return *p;
      } else if (bpp == 2) { // Uint16
        return *(Uint16 *)p;
      } else if (bpp == 3) { // Tuple of Uint8 RGB values
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
          return p[0] << 16 | p[1] << 8 | p[2];
        } else {
          return p[0] | p[1] << 8 | p[2] << 16;
        }
      } else if (bpp == 4) { // Uint32
        return *(Uint32 *)p;
      }
      return 0;
    }

    inline bool explicit_no_y_axis(const SDL_Rect &a, const SDL_Rect &b)
    {
      return a.y >= b.y + b.h || a.y + a.h <= b.y;
    }

    inline bool explicit_no_x_axis(const SDL_Rect &a, const SDL_Rect &b)
    {
      return a.x >= b.x + b.w || a.x + a.w <= b.x;
    }

    inline uint8_t explicit_x_axis(const int &ax, const int &aw, const int &bx, const int &bw)
    {
      // return ax >= bx + bw || ax + aw <= bx ? 0 : ax < bx ? COLLIDE_LEFT :
      // COLLIDE_RIGHT;
      return ax + aw > bx && ax < bx + bw ? ax < bx ? COLLIDE_LEFT : COLLIDE_RIGHT : 0;
    }

    inline uint8_t explicit_y_axis(const int &ay, const int &ah, const int &by, const int &bh)
    {
      // return ay >= by + bh || ay + ah <= by ? 0 : ay < by ? COLLIDE_BOTTOM :
      // COLLIDE_TOP;
      return (ay + ah > by && ay < by + bh) ? (ay < by ? COLLIDE_BOTTOM : COLLIDE_TOP) : 0;
    }

    inline uint8_t explicit_x_axis(const SDL_Rect &a, const SDL_Rect &b)
    {
      return explicit_x_axis(a.x, a.w, b.x, b.w);
    }

    inline uint8_t explicit_y_axis(const SDL_Rect &a, const SDL_Rect &b)
    {
      return explicit_y_axis(a.y, a.h, b.y, b.h);
    }
  } // namespace

  // Perfect pixel collision detection, adapted to tile sheets.
  // This is a true pixel detection, in other words it highly inefficient -- but
  // precise. A pre-generated vector with one bit (bool) could define if the
  // pixel is transparent or not, this would speed up things. This is best
  // generated when the image is read, it will take some space in primary
  // memory, but could exchange the stored pixel data if not longer needed.
  inline bool pixel_collision(SDL_Surface *a_surf, SDL_Surface *b_surf, const SDL_Rect &a,
                              const SDL_Rect &b, const SDL_Rect &a_src, const SDL_Rect &b_src)
  {
    SDL_PixelFormat *aFormat = a_surf->format;
    SDL_PixelFormat *bFormat = b_surf->format;
    int aBytesPerPixel = aFormat->BytesPerPixel;
    int bBytesPerPixel = bFormat->BytesPerPixel;
    int aPitch = a_surf->pitch;
    int bPitch = b_surf->pitch;
    void *aPixels = a_surf->pixels;
    void *bPixels = b_surf->pixels;
    Uint32 *aColorKey = nullptr;
    Uint32 *bColorKey = nullptr;
    SDL_GetColorKey(a_surf, aColorKey);
    SDL_GetColorKey(b_surf, bColorKey);

    // Get offset and adjust them to pixel areas in the tile sheet.
    int x1 = a_src.x - (a.x - std::max(a.x, b.x));
    int y1 = a_src.y - (a.y - std::max(a.y, b.y));
    int x2 = b_src.x - (b.x - std::max(a.x, b.x));
    int y2 = b_src.y - (b.y - std::max(a.y, b.y));
    int w = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
    int h = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);

    Uint8 aRed, aGreen, aBlue, aAlpha, bRed, bGreen, bBlue, bAlpha;
    for (int x = 0; x < w; ++x) {
      for (int y = 0; y < h; ++y) {
        Uint32 aPixel = get_pixel(aPixels, aBytesPerPixel, aPitch, x1 + x, y1 + y);
        Uint32 bPixel = get_pixel(bPixels, bBytesPerPixel, bPitch, x2 + x, y2 + y);
        SDL_GetRGBA(aPixel, aFormat, &aRed, &aGreen, &aBlue, &aAlpha);
        SDL_GetRGBA(bPixel, bFormat, &bRed, &bGreen, &bBlue, &bAlpha);
        if ((aAlpha != 0 && bAlpha != 0) ||
            (aColorKey && *aColorKey != aPixel && bColorKey && *bColorKey != bPixel)) {
          return true; // Found collision on pixels.
        }
      }
    }
    return false; // No pixel collision was found.
  }

  inline bool no_collision(const SDL_Rect &a, const SDL_Rect &b)
  {
    return explicit_no_x_axis(a, b) || explicit_no_y_axis(a, b);
  }

  inline uint8_t retrieve_horizontal_state(uint8_t state)
  {
    return state & 3U;
  }

  inline uint8_t retrieve_vertical_state(uint8_t state)
  {
    return state & (3U << 2);
  }

  inline uint8_t collision(const int &ax, const int &ay, const int &aw, const int &ah,
                           const int &bx, const int &by, const int &bw, const int &bh)
  {
    if (uint8_t horizontal = explicit_x_axis(ax, aw, bx, bw)) {
      if (uint8_t vertical = explicit_y_axis(ay, ah, by, bh)) {
        return horizontal | vertical;
      }
    }
    return 0;
  }

  // NOTE: the order is important for accurate state, if A is colliding left
  // with B then A should be on the LEFT SIDE of the parameter list -- if this
  // is not done the state will be a right collision, which is also correct
  // because now we compare B to A and not A to B.
  inline uint8_t collision(const SDL_Rect &a, const SDL_Rect &b)
  {
    return collision(a.x, a.y, a.w, a.h, b.x, b.y, b.w, b.h);
  }

  inline uint8_t horizontal(const SDL_Rect &a, const SDL_Rect &b)
  {
    return retrieve_vertical_state(collision(a, b));
  }

  inline uint8_t horizontal(const int &ax, const int &ay, const int &aw, const int &ah,
                            const int &bx, const int &by, const int &bw, const int &bh)
  {
    return retrieve_horizontal_state(collision(ax, ay, aw, ah, bx, by, bw, bh));
  }

  inline uint8_t vertical(const SDL_Rect &a, const SDL_Rect &b)
  {
    return retrieve_vertical_state(collision(a, b));
  }

  inline uint8_t vertical(const int &ax, const int &ay, const int &aw, const int &ah, const int &bx,
                          const int &by, const int &bw, const int &bh)
  {
    return retrieve_vertical_state(collision(ax, ay, aw, ah, bx, by, bw, bh));
  }
}; // namespace collision_utils

#endif
