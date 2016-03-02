#ifndef LIGHTINGPARAMETERS_H
#define LIGHTINGPARAMETERS_H
struct LightingParameters
{
    LightingParameters(void)
    {
         lightDir = Vector(0.6, 0.0, 0.8);
/*
         Ka = 0.3;
         Kd = 0.7;
         Ks = 5.3;
         alpha = 7.5;
*/
         Ka = 0.3;
         Kd = 0.7;
         Ks = 0.5;
         alpha = 7.5;
    }
  

    Vector lightDir; // The direction of the light source
    double Ka;           // The coefficient for ambient lighting.
    double Kd;           // The coefficient for diffuse lighting.
    double Ks;           // The coefficient for specular lighting.
    double alpha;        // The exponent term for specular lighting.
};
#endif
