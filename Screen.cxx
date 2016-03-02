#include "Screen.h"
#include <cstring>

Screen::Screen()
{ 
   depthMap = NULL;
}

Screen::~Screen()
{
   delete [] buffer;
   delete [] depthMap;
}

void Screen::SetSize(int w, int h)
{
   width = w;
   height = h;

   if(depthMap != NULL)
      delete [] depthMap;

   depthMap = new double[width*height];
   for(int i=0; i<width*height; i++) {
      depthMap[i] = -1;
   }

   mTransform.A[0][0] = width/2;
   mTransform.A[1][1] = height/2;
   mTransform.A[2][2] = 1;
   mTransform.A[3][3] = 1;

   mTransform.A[3][0] = width/2;
   mTransform.A[3][1] = height/2;
}

unsigned char * Screen::GetPixel(int x, int y)
{
   return buffer + (3*(width*y + x));
}

double Screen::GetDepth(int x, int y)
{
   return depthMap[y*width + x];
}

void Screen::SetDepth(int x, int y, double value) {
   depthMap[y*width +x] = value;
}

void Screen::Clear() {
   for(int i=0; i<width*height; i++) {
      buffer[3*i+0] = 255;
      buffer[3*i+1] = 255;
      buffer[3*i+2] = 255;
      depthMap[i] = -1.0;
   }
}

int Screen::GetWidth()  { return width;  }
int Screen::GetHeight() { return height; }
