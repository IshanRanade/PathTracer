#pragma once
#include <globals.h>
#include <scene/transform.h>
#include <raytracing/intersection.h>

class Intersection;
class Shape;

class Light
{
  public:
    virtual ~Light(){}
    Light(Transform t, const Color3f& Le,
          const std::shared_ptr<Shape>& shape)
        : transform(t), emittedLight(Le), shape(shape), name()
    {}



    // Returns the light emitted along a ray that does
    // not hit anything within the scene bounds.
    // Necessary if we want to support things like environment
    // maps, or other sources of light with infinite area.
    // The default implementation for general lights returns
    // no energy at all.
    virtual Color3f Le(const Ray &r) const;

    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const = 0;

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const = 0;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const = 0;


    QString name; // For debugging

    const Transform transform;
    const Color3f emittedLight;
    std::shared_ptr<Shape> shape;
};
