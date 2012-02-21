#ifndef UTIL_H
#define UTIL_H

#include "CImg.h"

using namespace std;
using namespace cimg_library;

unsigned char
  white[3]    = {255,255,255},
  white_111[3]= {1,1,1},
  black[3]    = {0,0,0},
  red[3]      = {255,0,0},
  grey[3]     = {127,127,127},
  green[3]    = {0,255,0},
  blue[3]     = {0,0,255},
  yellow[3]   = {255,255,0},
  magenta[3]  = {255,0,255},
  orange[3]   = {255,90,0},
  middle[3]   = {127,127,127},
  purple[3]   = {175,32,186}
;

typedef struct { unsigned char r, g, b; } rgb;
rgb colors[255];

// random color
rgb random_rgb(){
  rgb c;
  
  c.r = (unsigned char)(rand()%255);
  c.g = (unsigned char)(rand()%255);
  c.b = (unsigned char)(rand()%255);
  
  return c;
}

void randomColorForLabels()
{
  for (int i = 0; i < 255; i++)
    colors[i] = random_rgb();
}

CImg<unsigned char> colorizeLabelImage(CImg<unsigned char> image, bool randomColor=false)
{
  int width = image.width();
  int height= image.height();
  
  if (randomColor)
    randomColorForLabels();
  
  CImg<unsigned char> output(width, height, 1, 3, 0);
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int comp = image(x,y,0,0);
      output(x,y,0,0) = colors[comp].r;
      output(x,y,0,1) = colors[comp].g;
      output(x,y,0,2) = colors[comp].b;
    }
  }
  return output;
}

/* CImg gradients:
 * -1 = Backward finite differences
 * 0 = Centered finite differences
 * 1 = Forward finite differences
 * 2 = Using Sobel masks
 * 3 = Using rotation invariant masks
 * 4 = Using Deriche recusrsive filter. 
 */
template<typename T>
CImg<T> gradient_cimg(CImg<T> image, int type=2)
{
  CImgList<> gradList = image.get_gradient("xy",type);
  CImg<T> result = (gradList[0].pow(2) + gradList[1].pow(2)).sqrt().normalize(0,255);
  
  return result;
}

// Draw result image in the input image
template<typename T>
void draw_results(CImg<T> &image, CImg<T> &result, unsigned char* color)
{
  cimg_forXY(image,x,y) {
    if (result(x,y) == 255) {
      image(x,y,0) = color[0];
      image(x,y,1) = color[1];
      image(x,y,2) = color[2];
    }
  }
}

template < typename T >
void display_image_profileY(CImg<T>img, int mx, int my, CImgDisplay *profile) {
  if (!profile) return;
  
  const int mnx = mx*profile->width()/img.width();
  CImg<unsigned char>(profile->width(),profile->height(),1,3,0).
  draw_graph(img.get_shared_line(my,0,0),red,1,1,0,255,0).
  //draw_graph(img.get_shared_line(my,0,1),green,1,1,0,255,0).
  //draw_graph(img.get_shared_line(my,0,2),blue,1,1,0,255,0).
  draw_line(mnx,0,mnx,profile->height()-1,white,0.5f,cimg::rol(0xFF00FF00,mx%32)).
  draw_text(2,2,"(x,y)=(%d,%d)",white,0,1,13,mx,my).
  display(*profile);
}

#endif
