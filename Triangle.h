#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Screen.h"
#include "Vector.h"
#include "LightingParameters.h"
#include "Camera.h"

class Triangle
{
  private:
      // Metadata
      int            flatSide;
//      double         slopes[3];
//      double         xRankings[3];
      double         yRankings[3];

      void           FindVertexRankings();
      void           transform();
      void           resetSlopes();
      bool           isValidTriangle();
      void           FlipVertices(int, int);
 
      void           SplitAndRender(Screen &, Camera, LightingParameters &);
      // pass numer of times to rotate it in the opposite direction of its numbering
      // I.E. Clockwise numbering rotates Counter Clock-wise and vise versa 
      void           rotate(int);
      // transpose triangle around point 0
      void           flip(int);

      double         calculateShading(LightingParameters &, Camera, Vector);


  public:
      int            id;
      double         X[3];
      double         Y[3];
      double         Z[3];
      double         colors[3][3];
      Vector         normals[3];

                     Triangle();
   
      void           Print();
      void           ConvertToDeviceSpace(Screen &, Camera);
    
      void           render(Screen &, Camera, LightingParameters &);
};
#endif
