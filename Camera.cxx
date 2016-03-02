#include "Camera.h"
#include <math.h>
#include <stdio.h>

double SineParameterize(int curFrame, int nFrames, int ramp)
{
    int nNonRamp = nFrames-2*ramp;
    double height = 1./(nNonRamp + 4*ramp/M_PI);
    if (curFrame < ramp)
    {
        double factor = 2*height*ramp/M_PI;
        double eval = cos(M_PI/2*((double)curFrame)/ramp);
        return (1.-eval)*factor;
    }
    else if (curFrame > nFrames-ramp)
    {
        int amount_left = nFrames-curFrame;
        double factor = 2*height*ramp/M_PI;
        double eval =cos(M_PI/2*((double)amount_left/ramp));
        return 1. - (1-eval)*factor;
    }
    double amount_in_quad = ((double)curFrame-ramp);
    double quad_part = amount_in_quad*height;
    double curve_part = height*(2*ramp)/M_PI;
    return quad_part+curve_part;
}

Camera
GetCamera(int frame, int nframes)
{
    double t = SineParameterize(frame, nframes, nframes/10);
    Camera c;
    c.near = 5;
    c.far = 200;
    c.angle = M_PI/6;

    Vector pos(40*sin(2*M_PI*t), 40*cos(2*M_PI*t), 40);
    c.position = pos;

    Vector focus(0, 0, 0);
    c.focus = focus;

    Vector up(0, 1, 0);
    c.up = up;

    c.vTransform.A[0][0] = 1 / (tan(c.angle/2));
    c.vTransform.A[1][1] = 1 / (tan(c.angle/2));
    c.vTransform.A[2][2] = (c.far+c.near)/(c.far-c.near);
    c.vTransform.A[2][3] = -1;
    c.vTransform.A[3][2] = (2*c.far*c.near)/(c.far-c.near);

    Vector v = pos-focus;

    Vector v1 = up.cross(v).normalize();
    Vector v2 = v.cross(v1).normalize();
    Vector v3 = v.normalize();
    Vector Origin(0, 0, 0);

    c.camTransform.A[0][0] = v1.x;
    c.camTransform.A[0][1] = v2.x;
    c.camTransform.A[0][2] = v3.x;
    c.camTransform.A[0][3] = 0;

    c.camTransform.A[1][0] = v1.y;
    c.camTransform.A[1][1] = v2.y;
    c.camTransform.A[1][2] = v3.y;
    c.camTransform.A[1][3] = 0;

    c.camTransform.A[2][0] = v1.z;
    c.camTransform.A[2][1] = v2.z;
    c.camTransform.A[2][2] = v3.z;
    c.camTransform.A[2][3] = 0;

    c.camTransform.A[3][0] = v1.dot(Origin-pos);
    c.camTransform.A[3][1] = v2.dot(Origin-pos);
    c.camTransform.A[3][2] = v3.dot(Origin-pos);
    c.camTransform.A[3][3] = 1;

    return c;
}
