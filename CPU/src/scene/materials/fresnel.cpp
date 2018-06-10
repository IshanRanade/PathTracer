#include "fresnel.h"

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO
    cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);

    Float etaIPrime = etaI;
    Float etaTPrime = etaT;

    bool entering = cosThetaI > 0.f;
    if (!entering) {
       Float temp = etaIPrime;
       etaIPrime = etaTPrime;
       etaTPrime = temp;
       cosThetaI = std::abs(cosThetaI);
    }

    Float sinThetaI = std::sqrt(std::max((Float)0,
                                            1 - cosThetaI * cosThetaI));
    Float sinThetaT = etaI / etaT * sinThetaI;

    Float cosThetaT = std::sqrt(std::max((Float)0,
                                            1 - sinThetaT * sinThetaT));

    Float Rparl = ((etaTPrime * cosThetaI) - (etaIPrime * cosThetaT)) /
                 ((etaTPrime * cosThetaI) + (etaIPrime * cosThetaT));
    Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                 ((etaI * cosThetaI) + (etaT * cosThetaT));
    return Color3f((Rparl * Rparl + Rperp * Rperp) / 2);
}
