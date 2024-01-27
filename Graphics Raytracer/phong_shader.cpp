#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope scope;
    vec3 color;
    bool shadows = false; 
    vec3 diffuse_color;
    vec3 spec_color;
    
    if(render_world.ambient_color)
    {
        vec3 a =  render_world.ambient_intensity* render_world.ambient_color->Get_Color({});
        color= a * color_ambient->Get_Color({});
    }
    else
    {
        color = render_world.ambient_intensity * color_ambient->Get_Color({});
    }
    for (size_t j = 0; j <render_world.lights.size(); j++)
    {
        const Light* l_source = render_world.lights[j];
        vec3 direction = (l_source->position - intersection_point).normalized();
        double d = std::max(dot(normal, direction),0.0);
        if(render_world.enable_shadows)
        {
            Ray r_shadow(intersection_point + normal * 0.001, direction);
            Hit h_shadow = render_world.Closest_Intersection(r_shadow).second;
            vec3 position = l_source->position - intersection_point; 
            shadows = !(h_shadow.dist > position.magnitude() || !h_shadow.Valid());
        }
        if (!shadows && d>0)
        {
            vec3 i = l_source->Emitted_Light(l_source->position - intersection_point);
            vec3 v = -ray.direction.normalized();
            vec3 r = (2*d*normal - direction).normalized();
            color += d * (color_diffuse->Get_Color(hit.uv)*i);
            diffuse_color = color;
            
            double specular = std::max(dot(r,v),0.0); 
            if (specular > 0)
            {
                double f = pow(specular, specular_power);
                color += f * (i * color_specular->Get_Color(hit.uv));
                spec_color = color;
            }
        
        }
        Pixel_Print("shading for light L0: diffuse: (", diffuse_color, "); specular:(", spec_color, ")");
    }
    return color;
}
