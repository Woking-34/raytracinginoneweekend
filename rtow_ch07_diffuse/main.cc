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

#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
//#include "material.h"

// https://gist.github.com/mortennobel/8665258
#ifdef _WIN32
#include "drand48.h"
#endif

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5*color( ray(rec.p, target-rec.p), world);
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main() {
    int nx = 1200;
    int ny = 600;
    int ns = 100;

    std::ofstream file;
    file.open("rtow_ch07_diffuse.ppm", std::ios::out);
    file << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *list[2];
    list[0] = new sphere(vec3(0,0,-1), 0.5);
    list[1] = new sphere(vec3(0,-100.5,-1), 100);
    hitable *world = new hitable_list(list,2);
    camera cam;

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            file << ir << " " << ig << " " << ib << "\n";
        }
    }
}
