#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    if(twoSided || glm::dot(isect.normalGeometric, w) > 0) {
        return emittedLight;
    } else {
        return Color3f(0.0f);
    }
}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf) const {

    Intersection inter = shape->Sample(ref, xi, pdf);

    if(*pdf == 0.0 || glm::distance(inter.point, ref.point) < 0.0001) {
        return Color3f(0.0);
    }

    *wi = glm::normalize(inter.point - ref.point);

    return L(inter, -(*wi));
}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    Ray ray = ref.SpawnRay(wi);

    Intersection isectLight;
    if (!shape.get()->Intersect(ray, &isectLight)) {
        return 0;
    }

    float len2 = glm::distance2(ref.point, isectLight.point);
    float cosAngle = AbsDot(isectLight.normalGeometric, -wi);
    float pdf = 0.f;

    if(cosAngle >= 0.001) {
        pdf = (len2) / (cosAngle * shape.get()->Area());
    }

    return pdf;
}
