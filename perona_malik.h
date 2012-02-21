#ifndef PERONAMALIK_H
#define PERONAMALIK_H

//! Implementation based on "Mathematical Problems in Image Processing" book

//! 2D Perona-Malik equation
static double weight_perona_malik(double u) { return 1.0/std::sqrt(1.0+u*u); }

//! Implementation based on "Mathematical Problems in Image Processing" book
template < typename T >
void perona_malik2d_step(CImg<T> &img,CImg<T> &veloc, const float dt=0, const float delta=30.0)
{
  CImg_3x3(I,float);
  cimg_forC(img,k) cimg_for3x3(img,x,y,0,k,I,float)
  veloc(x,y,k) =
  weight_perona_malik((Inc-Icc)/delta)*(Inc-Icc)
  + weight_perona_malik((Ipc-Icc)/delta)*(Ipc-Icc)
  + weight_perona_malik((Icn-Icc)/delta)*(Icn-Icc)
  + weight_perona_malik((Icp-Icc)/delta)*(Icp-Icc);
  
  float xdt = dt;
  if (dt<0) {
    float m, M = veloc.max_min(m);
    xdt = -dt/cimg::abs(cimg::max(m,M));
  }
  
  img+=xdt*veloc;
}

//! Implementation based on "Mathematical Problems in Image Processing" book
/**
 * R *eturn an image that has been smoothed by the Perona-Malik flow PDE
 * \param nb_iter = Number of PDE iterations
 * \param dt = PDE time step. If dt<0, -dt represents the maximum PDE velocity that will be
 * applied on image pixels (adaptative time step)
 * \param delta = Discontinuity parameter
 * \param disp = Display used to show the PDE evolution. If disp==NULL, no display is performed.
 **/
template < typename T >
CImg<T> get_perona_malik2d(CImg<T> &ims, const int nb_iter=100, const float dt=0, const float delta=30.0, int show=0)
{
  CImg<float> img(ims), veloc(ims,false);
  
  bool run_ALG = true;
  CImgDisplay *disp = NULL, *profile = NULL;
  if (show) {
    disp = new CImgDisplay(img,"Progress",0);
    profile = new CImgDisplay(400,300,"Intensity Profile");
  }
  
  // Begin PDE iteration loop
  //-------------------------
  for (int iter=0; (iter<nb_iter || nb_iter==0); ) {
    
    // Compute one iteration of PDE explicit scheme
    if (run_ALG) {
      perona_malik2d_step(img,veloc,dt,delta);
      ++iter;
      if (show) CImg<>(img).draw_text(2,2,"iter = %d",white,0,1,13,iter).display(*disp);
    }
    
    if (show) {
      // Plot (R,G,B) intensity profiles and display it
      if (disp->mouse_x()>=0) display_image_profileY(img,disp->mouse_x(),disp->mouse_y(),profile);
      // Mouse button stops/starts PDE evolution.
      if (disp->button() || profile->button()) { /* disp->button() = profile->button() = 0; */ run_ALG = !run_ALG; }
      // When user close window finish computations
      if (disp->is_closed() || profile->is_closed() || disp->is_keyQ() || disp->is_keyESC() || profile->is_keyQ() || profile->is_keyESC()) {run_ALG=false; break; }
    }
  }
  while (show && run_ALG && !disp->is_closed() && !profile->is_closed() && !disp->is_keyQ() && !disp->is_keyESC() && !profile->is_keyQ() && !profile->is_keyESC()) {
    disp->wait();
    if (disp->mouse_x()>=0) display_image_profileY(img,disp->mouse_x(),disp->mouse_y(),profile);
  }
  
  return img;
}

#endif
