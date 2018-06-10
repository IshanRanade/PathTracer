#include "directlightingintegrator.h"
#include "../scene/lights/diffusearealight.h"

Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Color3f final = Color3f(0.0);

    Intersection intersection;
    if(scene.Intersect(ray, &intersection)) {
        Vector3f woW = -ray.direction;

        Color3f Le = intersection.Le(woW);
        final += Le;

        if(depth <= 0) {
            return final;
        }

        if(intersection.objectHit->GetMaterial() == nullptr) {
            return final;
        }

        intersection.objectHit->GetMaterial()->ProduceBSDF(&intersection);

        Vector2f xi = sampler.get()->Get2D();
        Vector3f wiW;
        float pdf;

        int lightIndex = int(scene.lights.size() * sampler.get()->Get1D());
        Color3f LiTerm = scene.lights[lightIndex].get()->Sample_Li(intersection, xi, &wiW, &pdf);

        Intersection shadowTestIntersection;
        bool didIntersect = scene.Intersect(intersection.SpawnRay(wiW), &shadowTestIntersection);

        if(didIntersect && shadowTestIntersection.objectHit->light != scene.lights[lightIndex]) {
            return final;
        }

        float lambertTerm = std::abs(glm::dot(wiW, intersection.normalGeometric));

        if(pdf < 0.0001) {
            return final;
        }

        Color3f frTerm = intersection.bsdf.get()->f(woW, wiW, BxDFType::BSDF_ALL);

        pdf /= scene.lights.size();

        Color3f tempColor = LiTerm * frTerm * lambertTerm;
        tempColor = tempColor / pdf;

        float heuristic1 = PowerHeuristic(1, pdf, 1, intersection.bsdf.get()->Pdf(woW, wiW));

        final = Le + heuristic1 * tempColor;

        // Shoot a second ray
        Intersection bsdfIntersection;
        Vector2f xiBSDF = sampler.get()->Get2D();
        float randomBSDF = sampler.get()->Get1D();
        Vector3f wiWBSDF;
        float pdfBSDF;
        BxDFType sampledTypeBSDF;
        Color3f sampledColorBSDF = intersection.bsdf.get()->Sample_f(woW, &wiWBSDF, xiBSDF, &pdfBSDF, BxDFType::BSDF_ALL, &sampledTypeBSDF, randomBSDF);

        Ray nextRay = intersection.SpawnRay(wiWBSDF);

        bool didIntersectBSDF = scene.Intersect(nextRay, &bsdfIntersection);

        if(didIntersectBSDF && bsdfIntersection.objectHit != nullptr && bsdfIntersection.objectHit->light == scene.lights[lightIndex]) {
            Color3f LiTermBSDF = scene.lights[lightIndex].get()->Sample_Li(intersection, xi, &wiW, &pdf);
            float lambertTermBSDF = std::abs(glm::dot(wiWBSDF, intersection.normalGeometric));

            sampledColorBSDF *= (LiTermBSDF * lambertTermBSDF / pdfBSDF);

            float pdf3 = ((DiffuseAreaLight*)(bsdfIntersection.objectHit->GetLight()))->Pdf_Li(intersection, wiWBSDF);

            if(pdf3 < 0.001) {
             return Le;
            }

            float heuristicBSDF = PowerHeuristic(1, pdfBSDF, 1, pdf3);
            final += (sampledColorBSDF * heuristicBSDF);
        }
    }
    return final;
}

float DirectLightingIntegrator::BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    //TODO
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float  DirectLightingIntegrator::PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    //TODO
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}
