#include <iostream>
#include <ostream>

#include "include/util.h"
#include "perona_malik.h"

int main(int argc,char **argv)
{
  cimg_usage("Perona--Malik anisotropic diffusion");
  
  const char* file_i  = cimg_option("-i", "input.bmp","Input image");
  const char* file_o  = cimg_option("-o", (char*)0,"Output image");
  int show            = cimg_option("-show",   1, "Show progres (0-no, 1-yes");
  const double dt     = cimg_option("-dt",  0.25, "Time step (if negative, automatic selection)");
  const int itm       = cimg_option("-it",   100, "Number of Iterations");
  const double dta    = cimg_option("-delta",  1, "Discontinuity parameter");
  
  CImg<float> image_in(file_i);
  CImg<float> image = image_in.resize(-100,-100,-100, 1); // Convert to grayscale image
  CImg<float> result(image);
  result.fill(0);
  
  CImgDisplay* disp_input=NULL;
  if (show==1) disp_input = new CImgDisplay(image, "Input image");
  
  result = get_perona_malik2d(image, itm, dt, dta, show);
  
  if (disp_input) {
    while (!disp_input->is_closed()) disp_input->wait();
  }
  
  if (file_o) result.save(file_o);
  return 0;
}
/*
 * Compilation:
 * g++ -o bin/app_perona_malik app_perona_malik.cpp -lX11 -lpthread -O3
 * 
 * Tests:
 * bin/app_perona_malik -i test_data/lenna_gray.bmp -show 2 -delta 1 -it 100
 */
