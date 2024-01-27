// Student Name: Irene Wu
// Student ID: iwu021 862176172 
#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    vec e_p = ray.endpoint - center; // (e-p)
    double a = 1;
    double b = 2 * dot(ray.direction,e_p);
    double c = dot(e_p, e_p) - (radius * radius);
    double disc = b*b - 4*a*c;
    double t_neg = (-b - sqrt(disc)) / (2*a);
    double t_pos = (-b + sqrt(disc)) / (2*a);
    if (t_neg > small_t && t_neg < t_pos)
    {
        return {t_neg, part};
    }
    if (t_pos > small_t)
    {
        return {t_pos, part}; 
    }
    return {-1,-1}; // no intersection 
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal = (ray.Point(hit.dist) - center).normalized();
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
