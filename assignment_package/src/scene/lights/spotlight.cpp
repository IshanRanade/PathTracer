#include "spotlight.h"

Color3f SpotLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    return emittedLight;
}

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                             Vector3f *wi, Float *pdf) const {

    Vector3f lightPos = glm::vec3(transform.T() * glm::vec4(0, 0, 0, 1));

    *wi = glm::normalize(lightPos - ref.point);

    *pdf = 1.f;

    float distanceSquared = glm::distance2(lightPos, ref.point);

    return emittedLight * Falloff(-*wi) / distanceSquared;
}

float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0;
}

float SpotLight::Falloff(const Vector3f &w) const {
    Vector3f wil = glm::vec3(transform.invT() * glm::vec4(w, 0));
    wil = glm::normalize(wil);

    float cosTheta = wil.z;

    if (cosTheta < cosTotalWidth) {
        return 0;
    }
    if (cosTheta > cosFalloffStart) {
        return 1;
    }
    float delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);

    return (delta * delta) * (delta * delta);
}
