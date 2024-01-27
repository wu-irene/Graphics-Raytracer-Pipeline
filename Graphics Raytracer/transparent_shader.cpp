#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope scope; 
    vec3 initial_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    if (recursion_depth >= render_world.recursion_depth_limit)
    {
        return initial_color;
    }
    vec3 d = (ray.endpoint - intersection_point).normalized();
    vec3 r = ( 2 * dot(d,normal) * normal - d).normalized();
    Ray reflection(intersection_point, r);
    Pixel_Print("Reflected ray: (end: (", reflection.endpoint, "; dir: ", reflection.direction, ")");
    vec3 r_color = render_world.Cast_Ray(reflection, recursion_depth +1);
    Pixel_Print("Reflected color: ", r_color);
    vec3 surface_normal;
    vec3 dir_refract;
    vec3 combine;
    double ratio;
    bool going_in = dot(ray.direction, normal) < 0;
    if (!going_in)
    {
        ratio = index_of_refraction;
        surface_normal = -normal;
    }
    else
    {
        ratio = 1/index_of_refraction;
        surface_normal = normal;
    }
    double cos = -dot(surface_normal, ray.direction);
    double k = 1 - ratio * ratio * (1 - cos * cos);
    if (k > 0)
    {
        dir_refract = ratio * ray.direction + (ratio * cos - sqrt(k)) * surface_normal;
        Ray refraction(intersection_point - 0.0001 * surface_normal, dir_refract);
        combine = render_world.Cast_Ray(refraction, recursion_depth +1);
    }
    else
    {
        combine = r_color;
        Pixel_Print("complete internal reflection\n","          final color (", combine, ")");
    }
    double R0 = pow((index_of_refraction -1)/(index_of_refraction +1),2);
    double reflect;
    if (k > 0)
    {
        reflect =  R0 + (1 - R0) * pow((1 - cos), 5);
    }
    else
    {
        reflect = 1.0;
    }
    vec3 color = reflect * r_color + (1-reflect) * combine;
    color = opacity * initial_color + (1 - opacity) * color;
    return color;
}
