// ray1.c — produces checkerboard.png and gradient.png (512x512, RGB)
// Falls back to writing into ./output/ if the current directory is not writable.

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #define MKDIR(path) mkdir(path, 0755)
#endif

static int try_write_png(const char *fname, int w, int h, int comp,
                         const void *data, int stride_bytes) {
  // Try current directory first
  if (stbi_write_png(fname, w, h, comp, data, stride_bytes)) return 1;

  // Fallback: write to ./output/<fname>
  (void)MKDIR("output");
  char alt[512];
  snprintf(alt, sizeof(alt), "output/%s", fname);
  if (stbi_write_png(alt, w, h, comp, data, stride_bytes)) {
    fprintf(stderr,
      "Note: wrote '%s' instead (current dir not writable?)\n", alt);
    return 1;
  }
  return 0;
}

static void make_checkerboard(unsigned char *img, int W, int H) {
  const int tile = 64; // 64x64 tiles, 8x8 board on 512x512
  for (int y = 0; y < H; ++y) {
    for (int x = 0; x < W; ++x) {
      int tileColor = ((x / tile) + (y / tile)) & 1; // 0 or 1
      unsigned char r = tileColor ? 0   : 255; // top-left must be red
      unsigned char g = 0;
      unsigned char b = tileColor ? 255 : 0;   // alternating blue
      int idx = (y * W + x) * 3;
      img[idx+0] = r;
      img[idx+1] = g;
      img[idx+2] = b;
    }
  }
}

static void make_gradient(unsigned char *img, int W, int H) {
  // EXACT math: 0..255 across 512 pixels => scale by 255/(W-1) and 255/(H-1)
  for (int y = 0; y < H; ++y) {
    for (int x = 0; x < W; ++x) {
      unsigned char r = (unsigned char)((x * 255) / (W - 1)); // left 0, right 255
      unsigned char g = (unsigned char)((y * 255) / (H - 1)); // top 0, bottom 255
      unsigned char b = 128;                                  // constant blue
      int idx = (y * W + x) * 3;
      img[idx+0] = r;
      img[idx+1] = g;
      img[idx+2] = b;
    }
  }
}

// (Optional) quick stochastic dithering demonstration.
// Not required for the assignment; writes gradient-dither.png if enabled below.
static void make_gradient_dither(unsigned char *img, int W, int H) {
  srand((unsigned)time(NULL));
  for (int y = 0; y < H; ++y) {
    for (int x = 0; x < W; ++x) {
      double r_exact = (double)x * 255.0 / (W - 1);
      double g_exact = (double)y * 255.0 / (H - 1);
      int r_floor = (int)floor(r_exact);
      int g_floor = (int)floor(g_exact);
      double r_frac = r_exact - r_floor;
      double g_frac = g_exact - g_floor;
      unsigned char r = ( (rand()/(double)RAND_MAX) < r_frac) ? (r_floor+1) : r_floor;
      unsigned char g = ( (rand()/(double)RAND_MAX) < g_frac) ? (g_floor+1) : g_floor;
      unsigned char b = 128;
      int idx = (y * W + x) * 3;
      img[idx+0] = (unsigned char)r;
      img[idx+1] = (unsigned char)g;
      img[idx+2] = b;
    }
  }
}

int main(void) {
  const int W = 512, H = 512, C = 3;
  const int stride = W * C;

  // We’ll reuse a single heap buffer (don’t use a huge stack array).
  unsigned char *img = (unsigned char*)malloc((size_t)W * H * C);
  if (!img) { fprintf(stderr, "Out of memory\n"); return 1; }

  // Ensure origin is the upper-left (default in stb); don’t flip.
  stbi_flip_vertically_on_write(0);

  // checkerboard.png
  make_checkerboard(img, W, H);
  if (!try_write_png("checkerboard.png", W, H, C, img, stride)) {
    fprintf(stderr, "Error: failed to write checkerboard.png\n");
    free(img);
    return 2;
  }

  // gradient.png
  make_gradient(img, W, H);
  if (!try_write_png("gradient.png", W, H, C, img, stride)) {
    fprintf(stderr, "Error: failed to write gradient.png\n");
    free(img);
    return 3;
  }

  // OPTIONAL bonus (uncomment to also write a dithered gradient)
  // make_gradient_dither(img, W, H);
  // if (!try_write_png("gradient-dither.png", W, H, C, img, stride)) {
  //   fprintf(stderr, "Warning: failed to write gradient-dither.png\n");
  // }

  free(img);
  return 0;
}
