#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
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
        float random = sampler.get()->Get2D()[0];
        Vector3f wiW;
        float pdf;
        BxDFType sampledType;
        Color3f sampledColor = intersection.bsdf.get()->Sample_f(woW, &wiW, xi, &pdf, BxDFType::BSDF_ALL, &sampledType, random);

        if(pdf < 0.0001) {
            return final;
        }

        float lambertTerm = std::abs(glm::dot(wiW, intersection.normalGeometric));

        Ray nextRay = intersection.SpawnRay(wiW);

        Color3f li = Li(nextRay, scene, sampler, --depth);

        Color3f tempColor = sampledColor * li * lambertTerm;

        tempColor = tempColor / pdf;

        if(sampledType & (BSDF_REFLECTION | BSDF_SPECULAR) ==  sampledType ||
          sampledType & (BSDF_TRANSMISSION | BSDF_SPECULAR) ==  sampledType ||
          sampledType & (BSDF_REFLECTION | BSDF_GLOSSY) ==  sampledType) {
            return final + li;
        } else {
            final += tempColor;
        }

    }
    return final;
}
