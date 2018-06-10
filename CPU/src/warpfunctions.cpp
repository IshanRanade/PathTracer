#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>
#include <pcg32.h>

glm::vec3 WarpFunctions::squareToDiskUniform(const glm::vec2 &sample)
{
    //TODO
    float r = std::sqrt(sample[0]);
    float theta = 2 * 3.14159265 * sample[1];
    return glm::vec3(r * std::cos(theta), r * std::sin(theta), 0);
}

glm::vec3 WarpFunctions::squareToDiskConcentric(const glm::vec2 &sample)
{
    //TODO
    float a = 2*sample[0]-1;
    float b = 2*sample[1]-1;

    float phi, r, u, v;
    if(a > -b) {
      if(a > b) {
        r = a;
        phi = 3.14159265 / 4.0 * (b/a);
      } else {
        r = b;
        phi = 3.14159265 / 4.0 * (2 - (a/b));
      }
    } else {
      if(a < b) {
        r = -a;
        phi = 3.14159265 / 4.0 * (4 + (b/a));
      } else {
        r = -b;
        if(b != 0) {
          phi = 3.14159265 / 4.0 * (6 - (a/b));
        } else {
          phi = 0;
        }
      }
    }

    u = r * cos(phi);
    v = r * sin(phi);
    return glm::vec3(u, v, 0);
}

float WarpFunctions::squareToDiskPDF(const glm::vec3 &sample)
{
    //TODO
    return 1.0 / (M_PI);
}

glm::vec3 WarpFunctions::squareToSphereUniform(const glm::vec2 &sample)
{
    //TODO
  float z = 1 - 2 * sample[0];
  float r = std::sqrt(std::max((float)0, (float)1.-z* z));
  float phi = 2 * 3.14159265 * sample[1];
  return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

float WarpFunctions::squareToSphereUniformPDF(const glm::vec3 &sample)
{
    //TODO
    return 1.0 / (4 * M_PI);
}

glm::vec3 WarpFunctions::squareToSphereCapUniform(const glm::vec2 &sample, float thetaMin)
{
    //TODO
    float z = 1 - 2.0 * ((180.0 - thetaMin)/180.0) * sample[0];
    float r = std::sqrt(std::max((float)0, (float)1.-z* z));
    float phi = 2 * 3.14159265 * sample[1];
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const glm::vec3 &sample, float thetaMin)
{
    //TODO
    return (0.5 * (1.0 / M_PI)) / (1.0 - std::cos(glm::radians(180.0 - thetaMin)));
}

glm::vec3 WarpFunctions::squareToHemisphereUniform(const glm::vec2 &sample)
{
    //TODO
    float z = sample[0];
    float r = std::sqrt(std::max((float)0, (float)1.-z* z));
    float phi = 2 * 3.14159265 * sample[1];
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const glm::vec3 &sample)
{
    //TODO
    return 2.0 * (1.0 / (4*M_PI));
}

glm::vec3 WarpFunctions::squareToHemisphereCosine(const glm::vec2 &sample)
{
    //TODO
    glm::vec3 d = squareToDiskConcentric(sample);
    float z = std::sqrt(std::max((float)0, 1 - d.x * d.x - d.y * d.y));
    return glm::vec3(d.x, d.y, z);
}

float WarpFunctions::squareToHemisphereCosinePDF(const glm::vec3 &sample)
{
    //TODO
    return sample.z * (1.0 / M_PI);
}
