#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    Hit inter_point;
    double d = dot(normal, ray.direction);
    if(fabs(d) < small_t)
    {
        return inter_point;
    }
    double t = dot(normal, x - ray.endpoint) / d;
    if (t < small_t)
    {
        return inter_point;
    }
    inter_point.dist = t;
    return inter_point;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
