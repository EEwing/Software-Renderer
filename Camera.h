#ifndef CAMERA_H
#define CAMERA_H
#include "Matrix.h"
#include "Vector.h"

class Camera
{
  public:
    double          near, far;
    double          angle;
    Vector          position;
    Vector          focus;
    Vector          up;

    Matrix          vTransform;
    Matrix          camTransform;
};

Camera GetCamera(int frame, int nframes);
#endif
