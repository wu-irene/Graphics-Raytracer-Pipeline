#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color = (1-reflectivity) * shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    if (recursion_depth >= render_world.recursion_depth_limit)
    {
        return color;
    }
    else
    {
        vec3 d = (ray.endpoint - intersection_point).normalized();
        vec3 r = ( 2 * dot(d,normal) * normal - d).normalized();
        Ray reflection(intersection_point + 0.0001 * normal, r);
        color += reflectivity * render_world.Cast_Ray(reflection, recursion_depth +1);
    }
    return color;
}

