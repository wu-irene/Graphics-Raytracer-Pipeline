#include "parse.h"
#include "texture.h"
#include "dump_png.h"

Texture::Texture(const Parse* parse,std::istream& in)
{
    std::string filename;
    in>>name>>filename>>use_bilinear_interpolation;
    Read_png(data,width,height,filename.c_str());
}

Texture::~Texture()
{
    delete [] data;
}

// uses nearest neighbor interpolation to determine color at texture coordinates
// (uv[0],uv[1]).  To match the reference image, details on how this mapping is
// done will matter.

vec3 Texture::Get_Color(const vec2& uv) const
{
    Debug_Scope scope; 
    Pixel_Print("Texture Initial UV: ", uv);
    float u = uv[0] - std::floor(uv[0]);
    float v = uv[1] - std::floor(uv[1]);
    Pixel_Print("Wrapped UV: ", u, " ", v);
    int i = std::floor(u * width);
    int j = std::floor(v * height);
    i = wrap(i, width);
    j = wrap(j, height);
    Pixel_Print("Indices: ", i, " ", j);
    Pixel pixel = data[j * width + i]; 
    Pixel_Print("Color: ", From_Pixel(pixel));
    return From_Pixel(pixel);
}



