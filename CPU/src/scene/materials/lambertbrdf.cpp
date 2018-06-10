#include "lambertbrdf.h"
#include <warpfunctions.h>
#include <samplers/sampler.h>

Color3f LambertBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return R * InvPi;
}

Color3f LambertBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    *sampledType = type;

    *wi = WarpFunctions::squareToHemisphereCosine(u);

    if(wo.z < 0) {
        wi->z = -wi->z;
    }

    *pdf = Pdf(wo, *wi);

    return f(wo, *wi);
}

float LambertBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return SameHemisphere(wo, wi) ? (AbsCosTheta(wi) * InvPi) : 0;
}
