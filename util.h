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

