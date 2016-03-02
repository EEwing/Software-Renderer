#include "Triangle.h"
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

using std::cerr;
using std::endl;


/***** Utility Functions *****/
double ceil441(double f)
{
    return ceil(f-0.00001);
}

double floor441(double f)
{
    return floor(f+0.00001);
}

double interpolate(double min, double mid, double max, double vMin, double vMax)
{
   double t = (mid-min)/(max-min);
   return vMin + t*(vMax-vMin);
}

Vector interpolate(double min, double mid, double max, Vector vMin, Vector vMax)
{
   double x = interpolate(min, mid, max, vMin.x, vMax.x);
   double y = interpolate(min, mid, max, vMin.y, vMax.y);
   double z = interpolate(min, mid, max, vMin.z, vMax.z);
   return Vector(x, y, z);
}

/***** Triangle Functions *****/
Triangle::Triangle()
{
   flatSide = -1;
   for(int i=0; i<3; i++) {
      X[i] = 0;
      Y[i] = 0;
      Z[i] = 0;
      yRankings[i] = Y[i];
      for(int j=0; j<3; j++)
         colors[i][j] = 0;
   }
}

void Triangle::Print() {
   char msg[1024];
   fprintf(stderr,  "Triangle #%d info: (X, Y, Z) (R, G, B)\n", id);
   for(int i=0; i<3; i++) {
     
      fprintf(stderr, "\t\e[33mVertex %d\e[m:\n\t\tCoords:\t(%f, %f, %f)\n", i, X[i], Y[i], Z[i]);
      fprintf(stderr, "\t\tColors:\tR=%f, G=%f, B=%f\n", colors[i][0], colors[i][1], colors[i][2]);
      fprintf(stderr, "\t\t%s\n", normals[i].toString());
   }
   
   fprintf(stderr, "\n");
}

void Triangle::FlipVertices(int v1, int v2) {
   double c[3];
   double x, y, z;
   Vector v;
   
   // Save Values
   x = X[v1];
   y = Y[v1];
   z = Z[v1];
   v = normals[v1];
   for(int i=0; i<3; i++)
      c[i] = colors[v1][i];

   // Set point 1
   X[v1] = X[v2];
   Y[v1] = Y[v2];
   Z[v1] = Z[v2];
   normals[v1] = normals[v2];

   // swap colors
   for(int i=0; i<3; i++) {
      colors[v1][i] = colors[v2][i];
      colors[v2][i] = c[i];
   }

   // set point 2
   X[v2] = x;
   Y[v2] = y;
   Z[v2] = z;
   normals[v2] = v;

   resetSlopes();
}

bool Triangle::isValidTriangle()
{
   for(int i=0; i<3; i++) {
      if(X[i] == X[(i+1)%3] && Y[i] == Y[(i+1)%3]) {
         return false; 
      }
   }
   return true;
}

void Triangle::FindVertexRankings()
{
   for(int i=0; i<3; i++) {
      yRankings[i] = Y[i];
   }
   for(int i=0; i<2; i++) {
      for(int j=2; j>i; j--) {
         if(yRankings[i] > yRankings[j]) {
            double y = yRankings[i];
            yRankings[i] = yRankings[j];
            yRankings[j] = y;
         }
      }
   }
}


void Triangle::rotate(int i)
{
   i %= 3;
   double x, y, z;
   double c[3];
   Vector v;

   for(int iters=0; iters<i; iters++) {
      x = X[0];
      y = Y[0];
      z = Z[0];
      v = normals[0];
      for(int j=0; j<3; j++) {
         c[j] = colors[0][j];
      }
      for(int j=0; j<2; j++) {
         X[j] = X[j+1];
         Y[j] = Y[j+1];
         Z[j] = Z[j+1];
         normals[j] = normals[j+1];
         for(int k=0; k<3; k++) {
            colors[j][k] = colors[j+1][k];
         }
      }
      X[2] = x;
      Y[2] = y;
      Z[2] = z;
      normals[2] = v;
      for(int j=0; j<3; j++) {
         colors[2][j] = c[j];
      }
   }
   resetSlopes();
}

void Triangle::flip(int axis)
{

   int i1, i2;
   i1 = (axis+1)%3;
   i2 = (axis+2)%3;
   
   FlipVertices(i1, i2);
}

void Triangle::resetSlopes()
{
   double run, rise;
   run = rise = 0;
   for(int i=0; i<3; i++) {
      run  = X[(i+1)%3] - X[i] ;
      rise = Y[(i+1)%3] - Y[i];
 
      double slope = rise/run;

      if( slope == 0 ) {
         flatSide = i;
      }
   }
}

void Triangle::transform()
{
   resetSlopes();
   if(flatSide == -1) {
      return;
   }
   if(flatSide != 0) {
      rotate(flatSide);
   }
   if(X[0] > X[1]) {
      flip(2);
   }
}

void Triangle::SplitAndRender(Screen &screen, Camera vAngle, LightingParameters &lp)
{
   FindVertexRankings();

   Triangle topTriangle;
   Triangle bottomTriangle;

   topTriangle.id = id;
   bottomTriangle.id = id;

   bottomTriangle.Y[0] = yRankings[1];
   bottomTriangle.Y[1] = yRankings[1];
   topTriangle.Y[0]    = yRankings[1];
   topTriangle.Y[1]    = yRankings[1];

   bottomTriangle.Y[2] = yRankings[0];
   topTriangle.Y[2]    = yRankings[2];

   int topPoint = 0;
   int botPoint = 0;
   int midPoint = 0;

   for(int i=0; i<3; i++) {
      if(Y[i] == yRankings[2]) {
         // top point
         topTriangle.X[2] = X[i];
         topPoint = i;
      } else if (Y[i] == yRankings[0]) {
         // bottom point
         bottomTriangle.X[2] = X[i];
         botPoint = i;
      } else {
         midPoint = i;
      }
   }

   topTriangle.X[0]    = X[midPoint];
   bottomTriangle.X[0] = X[midPoint];
   
   double xMid = interpolate(Y[botPoint], Y[midPoint], Y[topPoint], X[botPoint], X[topPoint]);
   Vector vMid = interpolate(Y[botPoint], Y[midPoint], Y[topPoint], normals[botPoint], normals[topPoint]);

   topTriangle.X[1]    = xMid;
   bottomTriangle.X[1] = xMid;

   topTriangle.normals[1]    = vMid;
   bottomTriangle.normals[1] = vMid;
     
   topTriangle.normals[0]    = normals[midPoint];
   topTriangle.normals[2]    = normals[topPoint];
   bottomTriangle.normals[0] = normals[midPoint];
   bottomTriangle.normals[2] = normals[botPoint];

   // Start setting colors
   for(int i=0; i<3; i++) {
      double c                    = interpolate(yRankings[0], yRankings[1], yRankings[2], colors[botPoint][i], colors[topPoint][i]);

      // Point that already existed
      topTriangle.colors[0][i]    = colors[midPoint][i];
      bottomTriangle.colors[0][i] = colors[midPoint][i];

      // top/bottom
      topTriangle.colors[2][i]    = colors[topPoint][i];
      bottomTriangle.colors[2][i] = colors[botPoint][i];

      // interpolated point
      topTriangle.colors[1][i]    = c;
      bottomTriangle.colors[1][i] = c;
   }

   // Set Z values
   topTriangle.Z[0] = Z[midPoint];
   topTriangle.Z[2] = Z[topPoint];
   bottomTriangle.Z[0] = Z[midPoint];
   bottomTriangle.Z[2] = Z[botPoint];

   topTriangle.Z[1] = bottomTriangle.Z[1] = interpolate(yRankings[0], yRankings[1], yRankings[2], Z[botPoint], Z[topPoint]);

   topTriangle.render(screen, vAngle, lp);
   bottomTriangle.render(screen, vAngle, lp);

}

double Triangle::calculateShading(LightingParameters &lp, Camera cam, Vector normal)
{

   Vector nNormal = normal.normalize();
   Vector nLight  = lp.lightDir.normalize();
   Vector nView   = (cam.position-cam.focus).normalize();

   // Not normalized dependent, gives same result no matter what
   double nDotL = normal.dot(nLight);   

   Vector R       = normal * 2 * (nDotL) - nLight;
   Vector nR      = R.normalize();

   double diffuse = fabs(nDotL);
   // cannot use normalized version of R
   double specular= std::max(0.0, pow(R.dot(nView), lp.alpha));

   //return (lp.Ka); /* just ambient */
   //return (lp.Kd * diffuse); /* just diffuse */
   //return (lp.Ks * specular); /* just specular */

   return (lp.Ka + (lp.Kd * diffuse) + (lp.Ks * specular)); /* all shadings */

}

void Triangle::ConvertToDeviceSpace(Screen &screen, Camera cam) {

   Matrix m = Matrix::ComposeMatrices(cam.camTransform, cam.vTransform);
   Matrix finalMatrix = Matrix::ComposeMatrices(m, screen.mTransform);

   double points[3][4];
   for(int i=0; i<3; i++) {
      points[i][0] = X[i];
      points[i][1] = Y[i];
      points[i][2] = Z[i];
      points[i][3] = 1;
   }
   double finalPoint[4];
   for(int i=0; i<3; i++) {
      finalMatrix.TransformPoint(points[i], finalPoint);
      X[i] = finalPoint[0]/finalPoint[3];
      Y[i] = finalPoint[1]/finalPoint[3];
      Z[i] = finalPoint[2]/finalPoint[3];
   } 

}

void Triangle::render(Screen &screen, Camera cam, LightingParameters &lp) {
   if(!isValidTriangle()) {
      return;
   }
   transform();
   if(flatSide == -1) {
      // This means the triangle isn't a flat-bottom/top triangle. split it up.
      SplitAndRender(screen, cam, lp);
      return;
   }

   double height  = Y[2]-Y[0];
   double offset  = 0.0;
   double rowMin  = 0.0;
   double rowMax  = 0.0;

   if(height < 0)
      offset = height;

   rowMin = ceil441(offset+Y[0]);
   rowMax = floor441(height-offset+Y[0]);

   for(int y=rowMin; y<=rowMax; y++) {
      double xMin, xMax;
      double zMin, zMax;
      double minColor[3];
      double maxColor[3];
      double leftEnd, rightEnd;

      if(y >= screen.GetHeight() || y < 0) { continue; }

      xMin = interpolate(Y[0], y, Y[2], X[0], X[2]);
      zMin = interpolate(Y[0], y, Y[2], Z[0], Z[2]);
      Vector normMin = interpolate(Y[0], y, Y[2], normals[0], normals[2]);

      xMax = interpolate(Y[2], y, Y[1], X[2], X[1]);
      zMax = interpolate(Y[2], y, Y[1], Z[2], Z[1]);
      Vector normMax = interpolate(Y[2], y, Y[1], normals[2], normals[1]);

      for(int k=0; k<3; k++) {
         minColor[k] = interpolate(Y[0], y, Y[2], colors[0][k], colors[2][k]);
         maxColor[k] = interpolate(Y[2], y, Y[1], colors[2][k], colors[1][k]);
      }

      if ( xMin >= screen.GetWidth() ) { continue; }
      leftEnd = std::max(0.0, (double) xMin);

      if(xMax < 0) { continue; }
      rightEnd = std::min((double) screen.GetWidth()-1.0, xMax);

      for (int x=ceil441(leftEnd); x<=floor441(rightEnd); x++) {

         unsigned char * buffer = screen.GetPixel(x, y);
         double zMid = interpolate(xMin, x, xMax, zMin, zMax);
         double c[3];

         Vector normPoint = interpolate(xMin, x, xMax, normMin, normMax);
         double shadingFactor = calculateShading(lp, cam, normPoint);

         for(int k=0; k<3; k++) {
             
             double base = interpolate(xMin, x, xMax, minColor[k], maxColor[k]);
             double finalColor = base * shadingFactor;

             finalColor = std::min(1.0, finalColor);
             finalColor = std::max(0.0, finalColor);
 
             c[k] = finalColor;
         }

         if(zMid > screen.GetDepth(x, y)) {
            buffer[0] = (unsigned char)(ceil441(c[0]*255.0));
            buffer[1] = (unsigned char)(ceil441(c[1]*255.0));
            buffer[2] = (unsigned char)(ceil441(c[2]*255.0));
            screen.SetDepth(x, y, zMid);
         }
      }
   }
}

