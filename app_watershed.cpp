#include <iostream>
#include <ostream>

#include "include/util.h"
#include "include/objects.h"
#include "watershed.h"

int main(int argc,char **argv)
{
  cimg_usage("Search Path");
  
  const char* file_i  = cimg_option("-i","input.bmp","Input image");
  const int blur      = cimg_option("-blur",2,"CImg blur value");
  //   float sigma         = cimg_option("-s", 1.0,"Sigma");
  
  CImg<unsigned char> image_in(file_i);
  CImg<unsigned char> image_gray = image_in.get_resize(-100,-100,-100,1); 
  CImg<unsigned char> image_rgb  = image_in.get_resize(-100,-100,-100,3); 
  
  CImg<unsigned char> blur_img = image_gray.get_blur(blur);
  //   CImg<unsigned char> blur_img(image);
  //   blur_img.fill(0);
  //   PGaussianFiltering2D(image, blur_img, sigma);
  CImg<unsigned char> gradient = gradient_cimg(blur_img, 2);
  CImg<unsigned char> gradient_norm = gradient.get_normalize(10,255);
  CImg<unsigned char> gradient_colr = gradient_cimg( image_rgb.get_blur(blur) , 2);
  
  int Dx = image_in.width();
  int Dy = image_in.height();
  CImg<unsigned char> canvas(image_rgb);
  CImg<unsigned char> gcanvas(gradient_colr);
  CImg<unsigned char> marker(Dx,Dy,1,1,0);
  CImg<unsigned char> marker_neg(Dx,Dy,1,1,0);
  CImg<unsigned char> gradmarker(Dx,Dy,1,1,0);
  
  CImg<signed int>    result(Dx,Dy,1,1,-1);
  CImg<unsigned char> result_ok0(Dx,Dy,1,1,0);
  CImg<unsigned char> result_ok1(Dx,Dy,1,3,0);
  CImg<unsigned char> result_ok2(Dx,Dy,1,1,0);
  
  CImgDisplay disp_canvas( cimg_fitscreen(canvas.width(),canvas.height(),canvas.depth()),  "Usage: L,R mouse, F1,F2,c" );
  CImgDisplay disp_result( cimg_fitscreen(canvas.width(),canvas.height(),canvas.depth()),  "Result" );
  CImgDisplay disp_gcanvas( cimg_fitscreen(canvas.width(),canvas.height(),canvas.depth()), "Gradient" );
  
  canvas.display(disp_canvas);
  gcanvas.display(disp_gcanvas);
  result_ok0.display(disp_result);
  
  CImg<unsigned char> marker_disp(disp_canvas.width(), disp_canvas.height(), 1, 3, 0);
  int circle_size1a = 3;
  int circle_size1b = (float) Dx / (float)disp_canvas.width() * circle_size1a + 0.5f;
  int circle_size2a = 5;
  int circle_size2b = (float) Dx / (float)disp_canvas.width() * circle_size2a + 0.5f;
  
  randomColorForLabels();
  
  int draw_type=1;
  bool clicked=false;
  volatile unsigned int btt=0;
  int mx=0, my=0, px=0, py=0, mouse_x=0, mouse_y=0, prev_x=0, prev_y=0;
  while (!disp_canvas.is_closed()) {
    disp_canvas.wait();
    
    if (disp_canvas.key()) {
      switch (disp_canvas.key()) {
        case cimg::keyF1:  draw_type=1; break;
        case cimg::keyF2:  draw_type=2; break;
        case cimg::keyC:
          marker.fill(0); marker_disp.fill(0);
          canvas.assign(image_rgb).display(disp_canvas);
          gcanvas.assign(gradient_colr).display(disp_gcanvas);
          break;
      }
      disp_canvas.set_key();
    }
    
    if (disp_canvas.mouse_x()>=0 && disp_canvas.mouse_y()>=0) {
      
      mx = disp_canvas.mouse_x(), 
      my = disp_canvas.mouse_y();
      mouse_x = mx*Dx/disp_canvas.width();
      mouse_y = my*Dy/disp_canvas.height();
      
      btt = disp_canvas.button();
      
      if (btt == 1) {
        if (draw_type==1) {
          marker.draw_line(mouse_x,mouse_y,prev_x,prev_y,white);
          marker_disp.draw_line(mx,my,px,py,red);
        }
        else {
          marker.draw_circle(mouse_x,mouse_y,circle_size1b,white);
          marker_disp.draw_circle(mx,my,circle_size1a,red);
        }
        draw_results(canvas,marker,red);  //canvas = canvas | marker;
        draw_results(gcanvas,marker,red); //gcanvas = gcanvas | marker;
        clicked=true;
      } else
        if (btt == 2) {
          marker.draw_circle(mouse_x,mouse_y,circle_size2b,black);
          marker_disp.draw_circle(mx,my,circle_size2a,white);
          canvas.assign(image_rgb); //.resize(disp_canvas);
          draw_results(canvas,marker,red);
          draw_results(canvas,result_ok2,yellow);
          gcanvas.assign(gradient_colr); //.resize(disp_gcanvas);
          draw_results(gcanvas,marker,red);
          draw_results(gcanvas,result_ok2,yellow);
          clicked=true;
        } else
          if (clicked) {
            cimg_forXY(marker,x,y) marker_neg(x,y) = 255-marker(x,y); // Invert marker images (lines should to be black)
            gradmarker = gradient_norm & marker_neg;                  // Add marker image to gradient image
            
            result.fill(-1);
            WatershedSegmentation(gradmarker, &result, 1, 0);         // Run marker-based watershed algorithm
            
            //result_ok0 = result.equalize(256,0,255);
            result_ok1 = colorizeLabelImage( result );
            result_ok2 = gradient_cimg(result,2);                     // Boundary detection
            cimg_forXY(result_ok2,x,y) result_ok2(x,y) = result_ok2(x,y)>0?255:0;
            canvas.assign(image_rgb); //.resize(disp_canvas);
            draw_results(canvas,marker,red);
            draw_results(canvas,result_ok2,yellow);
            gcanvas.assign(gradient_colr); //.resize(disp_gcanvas);
            draw_results(gcanvas,marker,red);
            draw_results(gcanvas,result_ok2,yellow);
            clicked=false;
          }
          
          prev_x = mouse_x; prev_y = mouse_y;
          px = mx; py = my;
          
          canvas.display(disp_canvas);
          gcanvas.display(disp_gcanvas);
          result_ok1.display(disp_result);
    }
  }

  return 0;
}

/*
 * Compilation:
 * g++ -o bin/app_watershed app_watershed.cpp -lX11 -lpthread -lm -O3
 * 
 * Tests:
 * bin/app_watershed -i test_data/lenna_gray.bmp
 */
