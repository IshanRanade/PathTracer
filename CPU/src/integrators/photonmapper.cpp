#include "photonmapper.h"
#include <scene/lights/diffusearealight.h>
#include <warpfunctions.h>
#include <integrators/fulllightingintegrator.h>

PhotonMapper::PhotonMapper(int numPhotons, std::vector<Photon> *photons, Scene *s, std::shared_ptr<Sampler> sampler, int recursionLimit, KDTree *kdtree, float searchRadius)
    : Integrator(Bounds2i(Point2i(0,0), Point2i(0,0)), s, sampler, recursionLimit), preprocessing(true), numPhotons(numPhotons), photons(photons), kdtree(kdtree), searchRadius(searchRadius)
{}

PhotonMapper::PhotonMapper(Bounds2i bounds, Scene *s, std::shared_ptr<Sampler> sampler, int recursionLimit, KDTree *kdtree, float searchRadius)
    : Integrator(bounds, s, sampler, recursionLimit), preprocessing(false), numPhotons(0), photons(nullptr), kdtree(kdtree), searchRadius(searchRadius)
{}


float getMaxEmittedLightChannel(Color3f color) {
    return std::max(std::max(color[0], color[1]), color[2]);
}

void PhotonMapper::Render()
{
    // PhotonMapper's Render() function has
    // two modes: when preprocessing, it traces
    // a collection of photons through the scene
    // and stores them in the given k-d tree.
    // If not preprocessing, it runs like a regular Integrator
    // and invokes Li().
    if(preprocessing)
    {
        // TODO
        // Determine how many photons to assign to each light source
        // given numPhotons and the intensity of each light.
        // Shoot a number of photons equal to numPhotons from
        // the lights, bouncing them through the scene and pushing
        // back the result of each bounce to the photons vector
        // stored in the PhotonMapper.

        // Find the total emitted light in the scene
        float totalEmittedLightSum = 0.0;
        for(std::shared_ptr<Light> light: scene->lights) {
            totalEmittedLightSum += getMaxEmittedLightChannel(((DiffuseAreaLight*) light.get())->emittedLight);
        }

        // Go through each light...
        for(std::shared_ptr<Light> light: scene->lights) {
            int lightCount = numPhotons * getMaxEmittedLightChannel(((DiffuseAreaLight*) light.get())->emittedLight) / totalEmittedLightSum;

            // Send out the correct number of protons
            for(int i = 0; i < lightCount; ++i) {
                Point2f xi = sampler->Get2D();
                float pdf;
                Intersection isect = ((DiffuseAreaLight*) light.get())->shape->Sample(xi, &pdf);

                Point2f sample = sampler->Get2D();
                Point3f direction = WarpFunctions::squareToHemisphereCosine(sample);
                Ray ray = isect.SpawnRay(direction);

                Color3f throughput = Color3f(1.0);
                int currentDepth = recursionLimit;

                float perPhotonIntensity = 4.0;
                Color3f accumulatedColor = ((DiffuseAreaLight*) light.get())->emittedLight / float(lightCount);
                accumulatedColor *= perPhotonIntensity;

                while(currentDepth > 0) {
                    Intersection newIsect;
                    if(scene->Intersect(ray, &newIsect)) {
                        // Hit a light, break
                        if(!newIsect.ProduceBSDF()) {
                            break;
                        }
                        // Otherwise, hit a real material

                        // If not first iteration
                        if(currentDepth < recursionLimit) {
                            Photon photon;
                            photon.color = accumulatedColor;
                            photon.pos = newIsect.point;
                            photon.wi = -ray.direction;

                            photons->push_back(photon);
                        }

                        Vector2f xiNew = sampler.get()->Get2D();
                        float randomNew = sampler.get()->Get1D();
                        Vector3f wiWNew;
                        float pdfNew;
                        BxDFType sampledTypeNew;
                        Color3f sampledColorNew = newIsect.bsdf.get()->Sample_f(-ray.direction, &wiWNew, xiNew, &pdfNew, BxDFType::BSDF_ALL, &sampledTypeNew, randomNew);

                        // Update the throughput
                        float throughputDot = std::abs(glm::dot(wiWNew, newIsect.normalGeometric));
                        throughput *= (sampledColorNew * throughputDot / pdfNew);

                        // Russian Roulette
                        float randomNumber = sampler->Get1D();
                        float maxChannel = std::max(std::max(throughput[0], throughput[1]), throughput[2]);

                        // Check if we should break
                        if(randomNumber < 1.0f - maxChannel) {
                            break;
                        }

                        accumulatedColor *= throughput;
                        ray = newIsect.SpawnRay(wiWNew);
                    } else {
                        break;
                    }

                    currentDepth--;
                }
            }
        }
    }
    else
    {
        Integrator::Render(); // Invokes Li for each ray from a pixel
    }
}

Color3f PhotonMapper::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
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

                    float pdf3 = ((DiffuseAreaLight*)(bsdfIntersection.objectHit->GetLight()))->Pdf_Li(intersection, wiWBSDF);

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

            if(!specularRay) {
                // Get the particles near this point
                std::vector<Photon> photonsArray = kdtree->particlesInSphere(intersection.point, searchRadius);

                Color3f photonColor;
                for(Photon photon: photonsArray) {

                    Vector2f xiPhoton = sampler.get()->Get2D();
                    float randomPhoton = sampler.get()->Get1D();
                    Vector3f wiWPhoton;
                    float pdfPhoton;
                    BxDFType sampledTypePhoton;
                    Color3f sampledColorPhoton = intersection.bsdf.get()->Sample_f(photon.wi, &wiWPhoton, xiPhoton, &pdfPhoton, BxDFType::BSDF_ALL, &sampledTypePhoton, randomPhoton);

                    float lambertTerm = std::abs(glm::dot(wiWPhoton, intersection.normalGeometric));

                    if(pdfPhoton > 0.0001) {
                        Color3f tempColor = photon.color * sampledColorPhoton * lambertTerm;
                        tempColor = tempColor / pdfPhoton;

                        photonColor += tempColor;
                    }

                }
                photonColor /= (M_PI * searchRadius * searchRadius);
                accumulatedColor += photonColor;
                break;
            }

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
