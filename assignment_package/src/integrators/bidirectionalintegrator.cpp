#include "bidirectionalintegrator.h"
#include <warpfunctions.h>
#include <string>

Color3f BidirectionalIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    // Store the intersection data of the ray coming from camera and the ray coming from a random light
    std::vector<Intersection> cameraIntersections;
    std::vector<Color3f> cameraThroughputs;
    std::vector<Intersection> lightIntersections;
    std::vector<Color3f> lightThroughputs;

    cameraIntersections.push_back(Intersection());
    cameraThroughputs.push_back(Color3f(1.0,1.0,1.0));

    // Start by tracing rays from the camera point into the scene and record intersections and throughputs
    Walk(&cameraIntersections, &cameraThroughputs, Ray(ray), depth, scene, sampler, Color3f(1.0));

    // Now pick a random light and trace a ray from the light
    int lightIndex = int(scene.lights.size() * sampler.get()->Get1D());

    float pdf1 = 1.0 / scene.lights.size();
    float pdf2;
    Intersection lightSampleIntersection = scene.lights[lightIndex]->shape->Sample(sampler->Get2D(), &pdf2);
    Vector3f randomDirection = WarpFunctions::squareToHemisphereCosine(sampler->Get2D());
    float pdf3 = WarpFunctions::squareToHemisphereCosinePDF(randomDirection);

    randomDirection = glm::normalize(scene.lights[lightIndex]->shape->transform.invTransT() * randomDirection);

    lightIntersections.push_back(Intersection());
    Color3f lightFirstThroughput = scene.lights[lightIndex]->L(lightSampleIntersection, randomDirection) / (pdf1 * pdf2 * pdf3 * sampler->samplesPerPixel);
    lightThroughputs.push_back(lightFirstThroughput);

    // Trace the rays coming from a random light
    Walk(&lightIntersections, &lightThroughputs, lightSampleIntersection.SpawnRay(randomDirection), depth, scene, sampler, lightFirstThroughput);

    Color3f accumulatedColor = Color3f(0.0);

    // If this ray immediately hit a light, return the color of the light
    if(cameraIntersections[1].objectHit->GetMaterial() == nullptr) {
        Vector2f xiLight = sampler.get()->Get2D();
        Vector3f wiWLight;
        float pdfLight;
        return scene.lights[lightIndex].get()->Sample_Li(cameraIntersections[1], xiLight, &wiWLight, &pdfLight);
    }

    // Now connect the two paths
    for(int i = 1; i < cameraIntersections.size(); ++i) {
        for(int j = 0; j < lightIntersections.size(); ++j) {
            if(j == 0) {
                // Do direct lighting
                Vector2f xiLight = sampler.get()->Get2D();
                Vector3f wiWLight;
                float pdfLight;

                Color3f LiTerm = scene.lights[lightIndex].get()->Sample_Li(cameraIntersections[i], xiLight, &wiWLight, &pdfLight);

                Vector3f woW = glm::normalize(cameraIntersections[i-1].point - cameraIntersections[i].point);

                Intersection shadowTestIntersection;
                bool didIntersect = scene.Intersect(cameraIntersections[i].SpawnRay(wiWLight), &shadowTestIntersection);

                if(didIntersect && pdfLight >= 0.0001 && shadowTestIntersection.objectHit->light == scene.lights[lightIndex]) {
                    float lambertTerm = std::abs(glm::dot(wiWLight, cameraIntersections[i].normalGeometric));
                    Color3f frTerm = cameraIntersections[i].bsdf.get()->f(woW, wiWLight, BxDFType::BSDF_ALL);
                    pdfLight /= scene.lights.size();

                    Color3f tempColor = (LiTerm * frTerm * lambertTerm) / pdfLight;

                    accumulatedColor += tempColor * cameraThroughputs[i];
                }
            } else {
                if(lightIntersections[j].bsdf != nullptr && cameraIntersections[i].bsdf != nullptr) {
                    // Connect the two paths

                    //Shadow test
                    Intersection shadowTestIntersection;
                    glm::vec4 shadowRayOrigin = glm::vec4(cameraIntersections[i].point[0], cameraIntersections[i].point[1], cameraIntersections[i].point[2], 1.0);
                    glm::vec4 shadowRayDirection = glm::normalize(glm::vec4(lightIntersections[j].point, 1.0) - glm::vec4(cameraIntersections[i].point, 1.0));
                    Ray shadowRay = Ray(shadowRayOrigin, shadowRayDirection);

                    bool didIntersect = scene.Intersect(shadowRay, &shadowTestIntersection);

                    // If we passed the shadow test then...
                    if(!didIntersect || shadowTestIntersection.objectHit->light == scene.lights[lightIndex]) {

                        Vector3f wiWCamera = glm::normalize(cameraIntersections[i-1].point - cameraIntersections[i].point);
                        Vector3f woWCamera = glm::normalize(lightIntersections[j].point - cameraIntersections[i].point);

                        Vector3f wiWLight = glm::normalize(cameraIntersections[i].point - lightIntersections[j].point);
                        Vector3f woWLight = glm::normalize(lightIntersections[j-1].point - lightIntersections[j].point);

                        Color3f fCamera = cameraIntersections[i].bsdf->f(woWCamera, wiWCamera);
                        Color3f fLight = lightIntersections[j].bsdf->f(woWLight, wiWLight);

                        float lambertCamera = std::abs(glm::dot(wiWCamera, cameraIntersections[i].normalGeometric));
                        float lambertLight = std::abs(glm::dot(wiWLight, lightIntersections[j].normalGeometric));

                        Color3f throughputCamera = cameraThroughputs[i];
                        Color3f throughputLight = lightThroughputs[j];
                        Color3f tempColor = fCamera * fLight * lambertCamera * lambertLight * throughputCamera * throughputLight;

                        accumulatedColor += tempColor;
                    }
                }
            }
        }
    }

    Color3f tmp = accumulatedColor;
    return accumulatedColor;
}

// Recursively trace a ray through the scene and save its intersection and throughput
void BidirectionalIntegrator::Walk(std::vector<Intersection> *intersectionsVector, std::vector<Color3f> *throughputsVector, Ray ray, int depth, const Scene &scene, std::shared_ptr<Sampler> sampler, Color3f throughput) const {
    if(depth > 0) {
        Intersection intersection;
        if(scene.Intersect(ray, &intersection)) {
            if(intersection.objectHit->GetMaterial() != nullptr) {
                intersection.objectHit->GetMaterial()->ProduceBSDF(&intersection);

                intersectionsVector->push_back(intersection);

                Vector3f woW = -ray.direction;
                Vector2f xiBSDF = sampler.get()->Get2D();
                float randomBSDF = sampler.get()->Get1D();
                Vector3f wiWBSDF;
                float pdfBSDF;
                BxDFType sampledTypeBSDF;
                Color3f sampledColorBSDF = intersection.bsdf.get()->Sample_f(woW, &wiWBSDF, xiBSDF, &pdfBSDF, BxDFType::BSDF_ALL, &sampledTypeBSDF, randomBSDF);

                float throughputDot = std::abs(glm::dot(wiWBSDF, intersection.normalGeometric));

                // Reduce the throughput for the next iteration so each iteration contributes less weight
                Color3f newThroughput = (throughput * sampledColorBSDF * throughputDot) / pdfBSDF;

                throughputsVector->push_back(newThroughput);

                Walk(intersectionsVector, throughputsVector, intersection.SpawnRay(wiWBSDF), depth - 1, scene, sampler, newThroughput);

            } else {
                intersectionsVector->push_back(intersection);
                throughputsVector->push_back(Color3f(0.0));
            }
        }
    }
}

float BidirectionalIntegrator::BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float  BidirectionalIntegrator::PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}
