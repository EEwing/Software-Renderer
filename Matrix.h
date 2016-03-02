#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>

class Matrix
{
  public:
    double          A[4][4];

    void            TransformPoint(const double *ptIn, double *ptOut);
    static Matrix   ComposeMatrices(const Matrix &, const Matrix &);
    void            Print(std::ostream &o);
   
                    Matrix();
};
#endif
