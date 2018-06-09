#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    Intersection isectLight = Sample(xi, pdf);

    Vector3f wi = glm::normalize(isectLight.point - ref.point);

    if (AbsDot(isectLight.normalGeometric, -wi) == 0.0) {
        *pdf = 0.0;
    } else {
        *pdf = (glm::distance(ref.point, isectLight.point) * glm::distance(ref.point, isectLight.point)) /
                (AbsDot(isectLight.normalGeometric, -wi) / *pdf);
    }

    return isectLight;
}
