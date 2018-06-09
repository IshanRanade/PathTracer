#include "fulllightingintegrator.h"
#include "../scene/lights/diffusearealight.h"

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Color3f accumulatedColor = Color3f(0.0);
    Color3f throughput = Color3f(1.0,1.0,1.0);
    int maxDepth = depth;

    bool specularRay = false;

    Ray copyRay = ray;

    while(depth > 0) {
        Intersection intersection;
        if(scene.Intersect(copyRay, &intersection)) {

            Vector3f woW = -copyRay.direction;

            Color3f MISColor = Color3f(0.f);

            if(depth == maxDepth || specularRay) {
                MISColor += intersection.Le(woW);
            }

            // If no this is a light, then add MISColor and continue to next iteration
            if(!intersection.ProduceBSDF()) {
                accumulatedColor += MISColor;
                break;
            }

            // BSDF Sampling
            Intersection bsdfIntersection;
            Vector2f xiBSDF = sampler.get()->Get2D();
            float randomBSDF = sampler.get()->Get1D();
            Vector3f wiWBSDF;
            float pdfBSDF;
            BxDFType sampledTypeBSDF;
            Color3f sampledColorBSDF = intersection.bsdf.get()->Sample_f(woW, &wiWBSDF, xiBSDF, &pdfBSDF, BxDFType::BSDF_ALL, &sampledTypeBSDF, randomBSDF);

            // Make sure the chosen material is not specular
            if((sampledTypeBSDF & BSDF_SPECULAR) == 0) {
                // Pick a random light
                Vector2f xiLight = sampler.get()->Get2D();
                Vector3f wiWLight;
                float pdfLight;

                // Pick a random light to sample
                int lightIndex = int(scene.lights.size() * sampler.get()->Get1D());
                Color3f LiTerm = scene.lights[lightIndex].get()->Sample_Li(intersection, xiLight, &wiWLight, &pdfLight);

                Intersection shadowTestIntersection;
                bool didIntersect = scene.Intersect(intersection.SpawnRay(wiWLight), &shadowTestIntersection);

                // Only continue if the new intersection intersected with the chosen light
                if(didIntersect) {
                    if(shadowTestIntersection.objectHit->light == scene.lights[lightIndex]) {
                        float lambertTerm = std::abs(glm::dot(wiWLight, intersection.normalGeometric));

                        if(pdfLight > 0.0001) {
                            Color3f frTerm = intersection.bsdf.get()->f(woW, wiWLight, BxDFType::BSDF_ALL);

                            pdfLight /= scene.lights.size();

                            Color3f tempColor = LiTerm * frTerm * lambertTerm;
                            tempColor = tempColor / pdfLight;

                            float heuristic1 = PowerHeuristic(1, pdfLight, 1, intersection.bsdf.get()->Pdf(woW, wiWLight));

                            MISColor = heuristic1 * tempColor;
                        }
                    }
                }

                // MIS Surface Sampling
                Ray nextRay = intersection.SpawnRay(wiWBSDF);

                bool didIntersectBSDF = scene.Intersect(nextRay, &bsdfIntersection);

                if(didIntersectBSDF && bsdfIntersection.objectHit != nullptr && bsdfIntersection.objectHit->light == scene.lights[lightIndex]) {
                    Color3f LiTermBSDF = scene.lights[lightIndex].get()->Sample_Li(intersection, xiLight, &wiWLight, &pdfLight);
                    float lambertTermBSDF = std::abs(glm::dot(wiWBSDF, intersection.normalGeometric));

                    sampledColorBSDF *= (LiTermBSDF * lambertTermBSDF / pdfBSDF);

                    float pdf3 = ((Light*)(bsdfIntersection.objectHit->GetLight()))->Pdf_Li(intersection, wiWBSDF);

                    if(pdf3 > 0.0001) {
                        float heuristicBSDF = PowerHeuristic(1, pdfBSDF, 1, pdf3);
                        MISColor += (sampledColorBSDF * heuristicBSDF);
                    }
                }
            }

            accumulatedColor += MISColor * throughput;

            // Create a new ray
            Vector2f xiNew = sampler.get()->Get2D();
            float randomNew = sampler.get()->Get1D();
            Vector3f wiWNew;
            float pdfNew;
            BxDFType sampledTypeNew;
            Color3f sampledColorNew = intersection.bsdf.get()->Sample_f(woW, &wiWNew, xiNew, &pdfNew, BxDFType::BSDF_ALL, &sampledTypeNew, randomNew);

            copyRay = intersection.SpawnRay(wiWNew);

            specularRay = (sampledTypeNew & BSDF_SPECULAR) != 0;

            // Update the throughput
            float throughputDot = std::abs(glm::dot(wiWNew, intersection.normalGeometric));
            throughput *= (sampledColorNew * throughputDot / pdfNew);

            // Russian Roulette
            float randomNumber = sampler->Get1D();
            float maxChannel = std::max(std::max(throughput[0], throughput[1]), throughput[2]);

            // Check if we should break
            if(randomNumber < 1.0f - maxChannel) {
                break;
            } else {
                throughput /= maxChannel;
            }

            depth--;
        }
        // If the ray did not intersect with anything then break out of while loop
        else {
            break;
        }
    }

    return accumulatedColor;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

