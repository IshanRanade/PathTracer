#pragma once
#include "integrator.h"

class BidirectionalIntegrator : public Integrator
{
public:
    BidirectionalIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const;

    void Walk(std::vector<Intersection> *intersectionsVector, std::vector<Color3f> *throughputsVector, Ray ray, int depth, const Scene &scene, std::shared_ptr<Sampler> sampler, Color3f throughput) const;

    float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const;
    float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const;
};

