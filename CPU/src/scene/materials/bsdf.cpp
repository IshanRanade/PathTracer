#include "bsdf.h"
#include <warpfunctions.h>

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
//TODO: Properly set worldToTangent and tangentToWorld
    : worldToTangent(glm::transpose(glm::mat3(isect.tangent, isect.bitangent, isect.normalGeometric))),
      tangentToWorld(glm::mat3(isect.tangent, isect.bitangent, isect.normalGeometric)),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{
}


void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent
    tangentToWorld = glm::mat3(t, b, n);
    worldToTangent = glm::transpose(tangentToWorld);
}


//
Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO
    Color3f sum = Color3f(0.f);

    for(int i = 0; i < numBxDFs; i++) {
        BxDF *currBxDf = bxdfs[i];
        if(currBxDf->MatchesFlags(flags)) {
            sum += currBxDf->f(worldToTangent * woW, worldToTangent * wiW);
        }
    }

    return sum;
}

// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.

// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.

// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.

// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.

// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum.

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType, float random) const
{
    //TODO
    BxDF* bxdf = bxdfs[int(random * numBxDFs)];
    Point2f tempXi = xi;

    Vector3f localWoW = worldToTangent * woW;
    Vector3f localWiW;

    Color3f savedColor = bxdf->Sample_f(localWoW, &localWiW, tempXi, pdf, sampledType);

    *wiW = tangentToWorld * localWiW;

    if(bxdf->type & (BSDF_REFLECTION | BSDF_SPECULAR) ==  bxdf->type ||
                bxdf->type & (BSDF_TRANSMISSION | BSDF_SPECULAR) ==  bxdf->type ||
                bxdf->type & (BSDF_REFLECTION | BSDF_GLOSSY) ==  bxdf->type){
            return savedColor;
        }

    float count = 1;
    for(int i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i] != bxdf) {
            *pdf += bxdfs[i]->Pdf(woW, *wiW);
            count++;
        }
    }

    *pdf = (*pdf / count);

    Color3f sum = Color3f(0.0);
    for(int i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i] != bxdf) {
            sum += bxdfs[i]->f(woW, *wiW);
        }
    }

    sum = sum + savedColor;

    return sum;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    //TODO
    Vector3f woWLocal = worldToTangent * woW;
    Vector3f wiWLocal = worldToTangent * wiW;

    float sum = 0;
    float count = 0;
    for(int i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i]->MatchesFlags(flags)) {
            sum += bxdfs[i]->Pdf(woWLocal, wiWLocal);
            count++;
        }
    }

    return sum / count;
}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    //TODO
    *sampledType = type;

    *wi = WarpFunctions::squareToHemisphereUniform(xi);

    if(wo.z < 0) {
        wi->z = -wi->z;
    }

    *pdf = Pdf(wo, *wi);

    return f(wo, *wi);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? (Inv2Pi) : 0;
}

BSDF::~BSDF()
{
    for(int i = 0; i < numBxDFs; i++)
    {
        delete bxdfs[i];
    }
}
