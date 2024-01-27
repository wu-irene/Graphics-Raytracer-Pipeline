#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "common.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris) {
    //depth-buffer to pass tests where triangles lie ahead or behind each other(hidden) 
    std::vector<double> dB(width * height, std::numeric_limits<double>::infinity());
    //iterate all 
    for (size_t i = 0; i < tris.size(); i++)
    {
        Triangle curr = tris[i];
        vec4 initial[3] = {curr.A, curr.B, curr.C};
        vec3 color;
        //viewpoint coordinates
        vec2 vpv[3];
        //transform vertices -> ndc -> viewpoint 
        for (int t = 0; t < 3; t++)
        {
            double w = initial[t][3];
            //perspective divide
            double x = initial[t][0] / w;
            double y = initial[t][1] / w;
            //to viewpoint 
            vpv[t][0] = x*(width/2) + ((width/2) - .5);
            vpv[t][1] = y*(height/2) + ((height/2)- .5);
        }
        //for simplification of lengthy calculations
        double dot0011 = vpv[0][0]*vpv[1][1];
        double dot0021 = vpv[0][0]*vpv[2][1];
        double dot1001 = vpv[1][0]*vpv[0][1]; 
        double dot1021 = vpv[1][0]*vpv[2][1];
        double dot2011 = vpv[2][0]*vpv[1][1];
        double dot2001 = vpv[2][0]*vpv[0][1];
        
        double area = ((dot1021 - dot2011) - (dot0021 - dot2001) + (dot0011- dot1001)) * .5;
        //bbox
        double max_x = std::max({vpv[0][0], vpv[1][0], vpv[2][0]});
        double max_y = std::max({vpv[0][1], vpv[1][1], vpv[2][1]});
        double min_x = std::min({vpv[0][0], vpv[1][0], vpv[2][0]});
        double min_y = std::min({vpv[0][1], vpv[1][1], vpv[2][1]});

        for (int x = min_x; x <= max_x; x++)
        {
            for (int y = min_y; y <= max_y; y++)
            {
                double a = (((dot1021 - dot2011) +(x * (vpv[1][1]-vpv[2][1])) + (y * (vpv[2][0] - vpv[1][0])))*.5)/area;
                double b= (((dot2001 - dot0021) + (x *(vpv[2][1] - vpv[0][1])) + (y * (vpv[0][0]-vpv[2][0])))* .5)/area;
                double g = (((dot0011 - dot1001) + (x * (vpv[0][1]-vpv[1][1])) + (y* (vpv[1][0]-vpv[0][0])))*.5)/area;
                if ((a>= 0) && (b >= 0) && (g >= 0))
                {
                    //pixel is in triangle! then calculate pers-correct inter
                    double div= 1/(a/initial[0][3]+b/initial[1][3]+g/initial[2][3]);
                    a = a* (div/initial[0][3]);
                    b = b * (div/initial[1][3]);
                    g = g* (div/initial[2][3]);
                    //interpolate to find the depth
                    double d = a * curr.A[2] + b * curr.B[2] + g * curr.C[2];
                    //send back color/update depth 
                    if ((a> 0) && (b > 0) && (g > 0) && (dB[(y * width) + x] > d))
                    {
                        color = a* curr.Ca + b * curr.Cb + g * curr.Cc;
                        set_pixel(pixels, width, height, x, y, color);
                        dB[(y * width + x)] = d;

                    }
                }
            }

        }
    }

}
