//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

// https://gist.github.com/mortennobel/8665258
#ifdef _WIN32
#include "drand48.h"
#endif

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
             return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}


hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48()); 
            if ((center-vec3(4,0.2,0)).length() > 0.9) { 
                if (choose_mat < 0.8) {  // diffuse

                    float r0 = drand48();
                    float r1 = drand48();
                    float r2 = drand48();
                    float r3 = drand48();
                    float r4 = drand48();
                    float r5 = drand48();

                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(r0*r1, r2*r3, r4*r5)));
                }
                else if (choose_mat < 0.95) { // metal

                    float r0 = drand48();
                    float r1 = drand48();
                    float r2 = drand48();
                    float r3 = drand48();

                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + r0), 0.5*(1 + r1), 0.5*(1 + r2)),  0.5*r3));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

void printUsageAndExit(const std::string& argv0)
{
    std::cerr << "\nUsage: " << argv0 << " [options]\n";
    std::cerr <<
        "App Options:\n"
        " --width              Render width\n"
        " --height             Render height\n"
        " --ns                 Number of samples per pixel\n"
        << std::endl;

    exit(1);
}

int main( int argc, char** argv ) {
    int nx = 1200;
    int ny = 800;
    int ns = 10;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);

        if (arg == "-h" || arg == "--help")
        {
            printUsageAndExit(argv[0]);
        }
        else if (arg == "--width")
        {
            if (i == argc - 1)
            {
                std::cerr << "Option '" << arg << "' requires additional argument.\n";
                printUsageAndExit(argv[0]);
            }
            nx = atoi(argv[++i]);
        }
        else if (arg == "--height")
        {
            if (i == argc - 1)
            {
                std::cerr << "Option '" << arg << "' requires additional argument.\n";
                printUsageAndExit(argv[0]);
            }
            ny = atoi(argv[++i]);
        }
        else if (arg == "--ns")
        {
            if (i == argc - 1)
            {
                std::cerr << "Option '" << arg << "' requires additional argument.\n";
                printUsageAndExit(argv[0]);
            }
            ns = atoi(argv[++i]);
        }
    }

    std::cout << "Render width: " << nx << std::endl;
    std::cout << "Render height: " << ny << std::endl;
    std::cout << "Number of samples per pixel: " << ns << std::endl << std::endl;

#ifdef _OPENMP
    int thread_num = omp_get_max_threads();
    std::cout << "omp_get_max_threads: " << thread_num << std::endl << std::endl;
#endif

    std::ofstream file;
    file.open("rtow.ppm", std::ios::out);
    file << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *list[5];
    float R = cos(M_PI/4);
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list,5);
    world = random_scene();

    int* buffer = new int[3 * nx*ny];

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for schedule(dynamic, 1)
    for (int j = ny - 1; j >= 0; j--) {
        fprintf(stdout, "\rRendering %5.2f%%", 100.0 * (ny-1-j) / (ny-1));
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]); 
            int ig = int(255.99*col[1]); 
            int ib = int(255.99*col[2]); 

            buffer[3*(i + (ny-1-j) * nx)+0] = ir;
            buffer[3*(i + (ny-1-j) * nx)+1] = ig;
            buffer[3*(i + (ny-1-j) * nx)+2] = ib;
        }
    }
    fprintf(stdout, "\n");

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta = stop - start;
    std::cout << "Render time: " << delta.count() / 1000.0 << " sec" << std::endl;

    std::cout << "Saving result to out.ppm" << std::endl;
    for (int pixel = 0; pixel < nx*ny; ++pixel)
    {
        file << buffer[3 * pixel + 0] << " " << buffer[3 * pixel + 1] << " " << buffer[3 * pixel + 2] << "\n";
    }
    std::cout << "Done!" << std::endl;
}