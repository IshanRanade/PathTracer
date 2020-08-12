#include "implicit.h"
#include <iostream>
#include <math.h>
#include <warpfunctions.h>
#include <string>

float Implicit::Area() const
{
    //DELETEME
    return 0.0; // We're assuming uniform scale
}

void Implicit::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    float epsilon = 0.0001;

    float dx = sceneSDF(Point3f(P[0] + epsilon, P[1], P[2])) - sceneSDF(Point3f(P[0] - epsilon, P[1], P[2]));
    float dy = sceneSDF(Point3f(P[0], P[1] + epsilon, P[2])) - sceneSDF(Point3f(P[0], P[1] - epsilon, P[2]));
    float dz = sceneSDF(Point3f(P[0], P[1], P[2] + epsilon)) - sceneSDF(Point3f(P[0], P[1], P[2] - epsilon));

    Vector3f normal = glm::normalize(Vector3f(dx, dy, dz));

    float tanX = P.x + epsilon;
    float tanY = P.y + epsilon;
    float tanZ = ((dx * (tanX - P.x) + dy * (tanY - P.y)) / (-dz)) + P.z;

    Vector3f tangent = glm::normalize(Vector3f(tanX, tanY, tanZ));
    Vector3f bitangent = glm::normalize(glm::cross(normal, tangent));

    *nor = glm::normalize(transform.invTransT() * normal);
    *tan = glm::normalize(glm::mat3(transform.T()) * tangent);
    *bit = glm::normalize(glm::mat3(transform.T()) * bitangent);
}

float torus(Point3f p) {
    glm::vec2 t = glm::vec2(1.0,0.5);
    glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x,p.z))-t.x,p.y);
    return glm::length(q)-t.y;
}

glm::vec3 opTwist( glm::vec3 p )
{
    float  c = std::cos(10.0*p.y+10.0);
    float  s = std::sin(10.0*p.y+10.0);
    glm::mat2   m = glm::mat2(c,-s,s,c);
    return glm::vec3(m*glm::vec2(p.x,p.z),p.y);
}

// A torus shape
float sdTorus( glm::vec3 p)
{
    glm::vec2 t = glm::vec2(0.80,0.25);
    return glm::length( glm::vec2(glm::length(glm::vec2(p.x,p.z))-t.x,p.y) )-t.y;
}

// A sphere
float sphere(Point3f p, float r) {
    return glm::length(p) - r;
}

// A rounded box
float udRoundBox(Point3f p) {
    p -= glm::vec3(0.0,0.2,0.0);
    glm::vec3 b = glm::vec3(0.15,0.15,0.15);
    float r = 0.35;
    glm::vec3 temp = glm::vec3(std::abs(p.x), std::abs(p.y), std::abs(p.z)) - b;
    temp = glm::vec3(std::max((float)0.0,temp.x), std::max((float)0.0,temp.y), std::max((float)0.0, temp.z));

    return std::max(-sphere(p, 0.55), glm::length(temp)-r);
}

// A twisty shape
float twisty(Point3f p) {
    return 0.2 * sdTorus(opTwist(p-glm::vec3(-3.0,-0.8, 0.0)));
}

// A ring torus shape
float ring( glm::vec3 p)
{
    glm::vec2 t = glm::vec2(1.2,0.10);
    return glm::length( glm::vec2(glm::length(glm::vec2(p.x,p.z))-t.x,p.y) )-t.y;
}

// The scene contains 4 different SDF objects
float Implicit::sceneSDF(Point3f p) const {
    if(type == 5) {
        glm::mat4 rotate = glm::rotate(glm::mat4(), (float)(80 * M_PI / 180.0), glm::vec3(1,0,0));
        rotate *= glm::rotate(glm::mat4(), (float)(0 * M_PI / 180.0), glm::vec3(0,1,0));
        rotate *= glm::rotate(glm::mat4(), (float)(-20 * M_PI / 180.0), glm::vec3(0,0,1));
        glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.0, 2.525, -1.2));
        float scale = 2.0;

        glm::vec3 q = glm::vec3(glm::inverse(translate * rotate) * glm::vec4(p.x, p.y, p.z, 1.0));
        q /= scale;

        return scale * (torus(q) + 0.03*std::sin(30.0*q.x)*std::sin(30.0*q.y)*std::sin(30.0*q.z));
    } else if(type == 1) {
        return twisty(p);
    } else if(type == 3) {
        glm::mat4 rotate = glm::rotate(glm::mat4(), (float)(0 * M_PI / 180.0), glm::vec3(1,0,0));
        rotate *= glm::rotate(glm::mat4(), (float)(-30 * M_PI / 180.0), glm::vec3(0,1,0));
        rotate *= glm::rotate(glm::mat4(), (float)(25 * M_PI / 180.0), glm::vec3(0,0,1));
        glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(3.2, 5.7, 0));
        float scale = 1.2;

        glm::vec3 q = glm::vec3(glm::inverse(translate * rotate) * glm::vec4(p.x, p.y, p.z, 1.0));

        return ring(q / scale) * scale;
    } else if(type == 2) {
        glm::mat4 rotate = glm::rotate(glm::mat4(), (float)(0 * M_PI / 180.0), glm::vec3(1,0,0));
        rotate *= glm::rotate(glm::mat4(), (float)(0 * M_PI / 180.0), glm::vec3(0,1,0));
        rotate *= glm::rotate(glm::mat4(), (float)(0 * M_PI / 180.0), glm::vec3(0,0,1));
        glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(3.2, 5.7, 0));
        float scale = 1.0;

        glm::vec3 q = glm::vec3(glm::inverse(translate * rotate) * glm::vec4(p.x, p.y, p.z, 1.0));

        return sphere(q / scale, 0.7) * scale;
    } else {
        return std::numeric_limits<float>::max();
    }
}

// Ray march into the scene using spherical ray marching
float Implicit::raymarch(glm::vec3 eye, glm::vec3 dir, float start, float end) const {
    float depth = start;
    for(int i = 0; i < 100; ++i) {
        float dist = sceneSDF(eye + depth * dir);
        if(dist < 0.000001) {
            return depth;
        }

        if(depth >= end) {
            return end;
        }

        depth += dist;
    }
    return end;
}

bool Implicit::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    float start = 0.0;
    float end = 120.0;

    float t = raymarch(r_loc.origin, r_loc.direction, start, end);

    if(t >= end) {
        return false;
    }

    Point3f p = r_loc.origin + t * r_loc.direction;
    InitializeIntersection(isect, t, p);
    return true;
}

// No need to do this, will not use implicit as a light source
Point2f Implicit::GetUVCoordinates(const Point3f &point) const
{
    //    Point3f p = glm::normalize(point);
    //    float phi = atan2f(p.z, p.x);
    //    if(phi < 0)
    //    {
    //        phi += TwoPi;
    //    }
    //    float theta = glm::acos(p.y);
    //    return Point2f(1 - phi/TwoPi, 1 - theta / Pi);
}

// No need to do this, will not use implicit as a light source
Intersection Implicit::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //    Point3f center = Point3f(transform.T() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    //    Vector3f centerToRef = glm::normalize(center - ref.point);
    //    Vector3f tan, bit;

    //    CoordinateSystem(centerToRef, &tan, &bit);

    //    Point3f pOrigin;
    //    if(glm::dot(center - ref.point, ref.normalGeometric) > 0)
    //        pOrigin = ref.point + ref.normalGeometric * RayEpsilon;
    //    else
    //        pOrigin = ref.point - ref.normalGeometric * RayEpsilon;

    //    if(glm::distance2(pOrigin, center) <= 1.f) // Radius is 1, so r^2 is also 1
    //        return Sample(xi, pdf);

    //    float sinThetaMax2 = 1 / glm::distance2(ref.point, center); // Again, radius is 1
    //    float cosThetaMax = std::sqrt(glm::max((float)0.0f, 1.0f - sinThetaMax2));
    //    float cosTheta = (1.0f - xi.x) + xi.x * cosThetaMax;
    //    float sinTheta = std::sqrt(glm::max((float)0, 1.0f- cosTheta * cosTheta));
    //    float phi = xi.y * 2.0f * Pi;

    //    float dc = glm::distance(ref.point, center);
    //    float ds = dc * cosTheta - glm::sqrt(glm::max((float)0.0f, 1 - dc * dc * sinTheta * sinTheta));

    //    float cosAlpha = (dc * dc + 1 - ds * ds) / (2 * dc * 1);
    //    float sinAlpha = glm::sqrt(glm::max((float)0.0f, 1.0f - cosAlpha * cosAlpha));

    //    Vector3f nObj = sinAlpha * glm::cos(phi) * -tan + sinAlpha * glm::sin(phi) * -bit + cosAlpha * -centerToRef;
    //    Point3f pObj = Point3f(nObj); // Would multiply by radius, but it is always 1 in object space

    //    Intersection isect;

    ////    pObj *= radius / glm::length(pObj); // pObj is already in object space with r = 1, so no need to perform this step

    //    isect.point = Point3f(transform.T() * glm::vec4(pObj.x, pObj.y, pObj.z, 1.0f));
    //    isect.normalGeometric = transform.invTransT() * Normal3f(nObj);

    //    *pdf = 1.0f / (2.0f * Pi * (1 - cosThetaMax));

    //    return isect;
}

// No need to do this, will not use implicit as a light source
Intersection Implicit::Sample(const Point2f &xi, Float *pdf) const
{
    //    Point3f pObj = WarpFunctions::squareToSphereUniform(xi);

    //    Intersection it;
    //    it.normalGeometric = glm::normalize( transform.invTransT() *pObj );
    //    it.point = Point3f(transform.T() * glm::vec4(pObj.x, pObj.y, pObj.z, 1.0f));

    //    *pdf = 1.0f / Area();

    //    return it;
}
