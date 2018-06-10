#include "pointlight.h"

Color3f PointLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    return emittedLight;
}

Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf) const {

    *wi = glm::normalize(glm::vec3(transform.T() * glm::vec4(0,0,0,1)) - ref.point);
    *pdf = 1.f;

    return emittedLight / glm::distance2(glm::vec3(transform.T() * glm::vec4(0,0,0,1)), ref.point);
}

float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0;
}


