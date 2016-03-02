#ifndef SCREEN_H
#define SCREEN_H
#include "Matrix.h"

class Screen
{
  private:
      int width, height;
  public:
      Screen();
     ~Screen();

      unsigned char   *buffer;
      double          *depthMap;
      Matrix           mTransform;
      
      int             GetWidth();
      int             GetHeight();
      void            SetSize(int, int);

      unsigned char * GetPixel(int, int);
      double          GetDepth(int, int);
      void            SetDepth(int, int, double);

      void            Clear();
};
#endif
