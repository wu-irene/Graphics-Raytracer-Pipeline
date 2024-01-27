#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include <limits> 
extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Debug_Scope scope;
    double min_t = std::numeric_limits<double>::max();
    Hit closest_hit; 
    Shaded_Object o;
    for (size_t i = 0; i < objects.size(); i++)
    {
        Shaded_Object temp = objects[i]; 
        Hit temp2 = temp.object->Intersection(ray, 0);
        if (temp2.dist >= small_t)
        {
            if (temp2.dist < min_t)
            {
                Pixel_Print("   intersect test with ", temp.object->name, "; hit (dist: ", temp2.dist, "; triangle: ", temp2.triangle, "; uv:", temp2.uv, ")" ); //im not sure why my triangle is 0 rather than -1 since we havent implemented any meshes yet 
                min_t = temp2.dist;
                closest_hit = temp2;
                o = temp; 
                
            }
        }
        else if (temp2.dist == -1)
        {
            Pixel_Print("   no intersection with ", temp.object->name);
        }
        if (closest_hit.Valid()) //to fix pixel trace printout sequence 
        {
            Pixel_Print("   closest intersection; obj: ", o.object->name, "; hit: (dist: ", closest_hit.dist, "; triangle: ", closest_hit.triangle, "; uv ", closest_hit.uv, ")");
        }
    }
    return {o, closest_hit};
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Debug_Scope scope; 
    Pixel_Print("debug pixel: -x ", pixel_index[0], " -y ", pixel_index[1]);
    vec3 end_point = camera.position; 
    vec3 w_position = camera.World_Position(pixel_index);
    vec3 direction = (w_position - end_point).normalized(); 
    Ray ray(end_point, direction);
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    Debug_Scope scope; 
    Pixel_Print("cast ray (end: ", ray.endpoint, "; dir: ", ray.direction, ")");
    vec3 color;
    std::pair<Shaded_Object, Hit> closest_pair = Closest_Intersection(ray); 
    Hit c_hit = closest_pair.second; 
    Shaded_Object c_object = closest_pair.first;

    if (c_hit.Valid()) // if there is an intersection 
    {
        vec3 i_point = ray.Point(c_hit.dist);
        vec3 normal_at_point = c_object.object->Normal(ray, c_hit);
        color = c_object.shader->Shade_Surface(*this, ray, c_hit, i_point, normal_at_point, recursion_depth);

        Pixel_Print("call Shade_Surface with location ", i_point, "; normal: ", normal_at_point);
        Pixel_Print("   flat shader; color: ", color);
    }
    else
    {
        if (background_shader) //check if not null
        {
            color = background_shader->Shade_Surface(*this, ray, Hit(), vec3(0,0,0), vec3(0,0,0), recursion_depth);
        }
        else
        {
            color = vec3(0,0,0); //return black
        }
    }
    return color;
}
