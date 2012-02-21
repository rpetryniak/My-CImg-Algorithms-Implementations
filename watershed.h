#ifndef WATERSHED_H
#define WATERSHED_H

#include "include/objects.h"
#include <vector>

/**
 Simplified implementation of Vincent-Soilles watershed algorithm,
 with 8-neighbor connectedness.
 You will probably want to call Watershed function with a quantized version
 of the gradient magnitude of the original image.
 
 @author Stan Birchfield
 */


int iGetLabelNeighbor(const PixelNode& pt, const CImg<signed int>& labels)
{
  int w = labels.width() -1;
  int h = labels.height() -1;
  int tmp;
  if (pt.getX() > 0) { tmp = labels(pt.getX()-1, pt.getY());  if (tmp >= 0)  return tmp; }
  if (pt.getY() > 0) { tmp = labels(pt.getX(), pt.getY()-1);  if (tmp >= 0)  return tmp; }
  if (pt.getX() < w) { tmp = labels(pt.getX()+1, pt.getY());  if (tmp >= 0)  return tmp; }
  if (pt.getY() < h) { tmp = labels(pt.getX(), pt.getY()+1);  if (tmp >= 0)  return tmp; }
  return -1;
}

void iExpand(int x, int y, unsigned char g, int label,
            const CImg<unsigned char>& img, CImg<signed int>* labels,
            std::vector<PixelNode>* frontier)
{
  // <= is necessary for marker-based and does not harm non-marker-based
  if ( img(x, y) <= g && (*labels)(x, y) < 0)
  {
    (*labels)(x, y) = label;
    frontier->push_back( PixelNode(x, y) );
  }
}

inline void iExpand2(CImg<unsigned char>& img, CImg<signed int>* out, std::vector<PixelNode>* frontier, const PixelNode& p, int color, int new_color)
{
  int pix  = img(p.getX(), p.getY());
  int pix2 = (*out)(p.getX(), p.getY());
  if (pix == color && pix2 != new_color)
  {
    frontier->push_back(p);
    (*out)(p.getX(), p.getY()) = new_color;
  }
}

//template <typename T>
void FloodFill8(CImg<unsigned char>& img, int x, int y, int new_color, CImg<signed int>* out)
{
  if (x<0 || y<0 || x>=img.width() || y>=img.height())  printf("Out of bounds");

  std::vector<PixelNode> frontier;
  int w = img.width() -1;
  int h = img.height()-1;
  PixelNode p;

  //if (new_color == (*out)(x,y))  return;
  int color = img(x, y);
  frontier.push_back( PixelNode(x,y) );
  (*out)(x, y) = new_color;

  while ( !frontier.empty() )
  {
    p = frontier.back();
    frontier.pop_back();

    if (p.getX() > 0)  iExpand2(img, out, &frontier, PixelNode(p.getX()-1, p.getY()  ), color, new_color);
    if (p.getX() < w)  iExpand2(img, out, &frontier, PixelNode(p.getX()+1, p.getY()  ), color, new_color);
    if (p.getY() > 0)  iExpand2(img, out, &frontier, PixelNode(p.getX(),   p.getY()-1), color, new_color);
    if (p.getY() < h)  iExpand2(img, out, &frontier, PixelNode(p.getX(),   p.getY()+1), color, new_color);

    if (p.getX() > 0 && p.getY() > 0)  iExpand2(img, out, &frontier, PixelNode(p.getX()-1, p.getY()-1), color, new_color);
    if (p.getX() > 0 && p.getY() < h)  iExpand2(img, out, &frontier, PixelNode(p.getX()-1, p.getY()+1), color, new_color);
    if (p.getX() < w && p.getY() > 0)  iExpand2(img, out, &frontier, PixelNode(p.getX()+1, p.getY()-1), color, new_color);
    if (p.getX() < w && p.getY() < h)  iExpand2(img, out, &frontier, PixelNode(p.getX()+1, p.getY()+1), color, new_color);
  }
}


//template<typename T>
void WatershedSegmentation(CImg<unsigned char> &img, CImg<signed int> *labels, bool marker_based, int show)
{
  int next_label  = 0;
  const int ngray = 256;

  CImgDisplay* disp_progress;

  if (show)
    disp_progress = new CImgDisplay(*labels, "Progress",1);

  // Precompute array of pixels for each graylevel
  // (pixels[g] contains the coordinates of all the pixels with graylevel g)
  std::vector<PixelNode> pixels[ngray];
  {
    for (int y=0 ; y<img.height() ; y++)
      for (int x=0 ; x<img.width() ; x++)
        pixels[ img(x,y) ].push_back( PixelNode(x, y) );
  }

  // For each graylevel, ...
  for (int g=0 ; g<ngray ; g++)
  {
    //printf("level=%d\n",g);
    // Create frontier of all the pixels with graylevel g that border a pixel in an existing catchment basin
    std::vector<PixelNode> frontier, frontier2;
    int i;
    for (i=0 ; i<pixels[g].size() ; i++)
    {
      const PixelNode& pt = pixels[g][i];
      int lab = iGetLabelNeighbor(pt, *labels);
      if (lab >= 0)
      {
        (*labels)(pt.getX(), pt.getY()) = lab;
        frontier.push_back( pt );
      }
    }

    // Expand all the pixels on the frontier, moving a distance of one pixel
    // each time to correctly handle the situation when a region is adjacent
    // to more than one catchment basin.

    do
    {
      while (frontier.size() > 0)
      {
        PixelNode pt = frontier.at(frontier.size()-1);
        frontier.pop_back();

        int lab = (*labels)(pt.getX(), pt.getY());
        const int w = img.width() -1;
        const int h = img.height()-1;

        if (pt.getX() > 0)                  iExpand(pt.getX()-1, pt.getY(),   g, lab, img, labels, &frontier2);
        if (pt.getX() < w)                  iExpand(pt.getX()+1, pt.getY(),   g, lab, img, labels, &frontier2);
        if (pt.getY() > 0)                  iExpand(pt.getX(),   pt.getY()-1, g, lab, img, labels, &frontier2);
        if (pt.getY() < h)                  iExpand(pt.getX(),   pt.getY()+1, g, lab, img, labels, &frontier2);

        if (pt.getX() > 0 && pt.getY() > 0) iExpand(pt.getX()-1, pt.getY()-1, g, lab, img, labels, &frontier2);
        if (pt.getX() < w && pt.getY() < h) iExpand(pt.getX()+1, pt.getY()+1, g, lab, img, labels, &frontier2);
        if (pt.getX() > 0 && pt.getY() < h) iExpand(pt.getX()-1, pt.getY()+1, g, lab, img, labels, &frontier2);
        if (pt.getX() < w && pt.getY() > 0) iExpand(pt.getX()+1, pt.getY()-1, g, lab, img, labels, &frontier2);
      }
      frontier = frontier2;
      frontier2.clear();
    }
    while (frontier.size() > 0);

    if (show) {
      labels->display(*disp_progress);
      disp_progress->wait(50);
    }

    // For each connected region with graylevel g that does not yet belong to a
    // catchment basin, declare a new catchment basin

    if (!marker_based || g==0)
    {
      for (i=0 ; i<pixels[g].size() ; i++)
      {
        const PixelNode& pt = pixels[g][i];
        if ((*labels)(pt.getX(), pt.getY()) < 0)
        {
          FloodFill8(img, pt.getX(), pt.getY(), next_label++, labels);
          if (show) {
            labels->display(*disp_progress);
            disp_progress->wait(10);
          }
          //while (!disp_progress.button());
          //disp_progress.button() = 0;
        }
      }
      //if (show) while (!disp_progress->is_closed());
    }
  }
  if (show) while (!disp_progress->is_closed());
}

#endif
