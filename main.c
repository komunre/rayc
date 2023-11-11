#include <cstdio>
#include <string.h>
#include <cmath>

#include "tgaimage.h"

#define MAX_X 80
#define MAX_Y 25

#define DEGREE_TO_RAD 0.0174
 
const TGAColor white = {255, 255, 255, 255};
const TGAColor blue   = {255, 0, 0, 255};
const TGAColor red  = {0, 0, 255, 255};
const TGAColor green = {0, 255, 0, 255};
const TGAColor black = {0, 0, 0, 255};

char vmem[80*25];

#define MAP_SIZE_X 1000
#define MAP_SIZE_Y 1000
char map[MAP_SIZE_X*MAP_SIZE_Y];

float DegreesToRadians(float degrees) {
    return degrees * DEGREE_TO_RAD;
}


float player_a = 1 * DEGREE_TO_RAD;
float player_x = 50;
float player_y = 50;
int fov = 30;

float RayCast(float sx, float sy, float a, float maxDist) {
    float c = 0;
    for (; c < maxDist; c += 0.5) {
        float x = sx + c*cos(a);
        float y = sy + c*sin(a);

        if (x > MAP_SIZE_X || y > MAP_SIZE_Y || x < 0 || y < 0) break;

        if (map[(int)x + ((int)y*MAP_SIZE_X)] != '-') break;
    }
    return c;
}

//void line(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor color) {
//    for (float t=0; t<1; t+=0.01) {
//        int x = x0 + (x1-x0)*t;
//        int y = y0 + (y1-y0)*t;
//        image->set(x, y, color);
//    }
//}

void line(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor color) {
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    float derror = std::abs(dy/float(dx)); 
    float error = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image->set(y, x, color); 
        } else { 
            image->set(x, y, color); 
        } 
        error += derror; 
        if (error>.5) { 
            y += (y1>y0?1:-1); 
            error -= 1.; 
        } 
    } 
}

int main(int argc, char** argv) {
    TGAImage debug(1920, 1080, TGAImage::RGB);
    debug.set(player_x, player_y, red);

    TGAImage frame(512, 512, TGAImage::RGB);

    printf("Hello????\n");

    for (int i = 0; i < MAP_SIZE_X*MAP_SIZE_Y; i++) {
        int y = i/MAP_SIZE_X;
        int x = i - y*MAP_SIZE_X;
        if (x > 400 && x < 600 && y > 200 && y < 230) {
            map[i] = 'w';
        }
        else if (x > 100 && x < 250 && y > 80 && y < 90) {
            map[i] = 'w';
        }
        else {
            map[i] = '-';
        }
    }

    for (int i = 0; i < MAP_SIZE_X*MAP_SIZE_Y; i++) {
        int y = i/MAP_SIZE_X;
        int x = i - y*MAP_SIZE_X;
        if (map[i] != '-') debug.set(x, y, blue);
    }

    int f = 0;
    char* name = (char*)malloc(sizeof(char)*40);
    float additive = -fov/2;
    while (player_a < 359 * DEGREE_TO_RAD) {
    
    for (int i = 0; i < 512*512; i++) {
        int y = i/512;
        int x = i - y*512;
        frame.set(x, y, black);
    }

    for (int i = 0; i < 512; i++) {
        float trueA = player_a + DegreesToRadians(additive+(fov*((float)i/512)));
        float t = RayCast(player_x, player_y, trueA, 10000);

        float height = (float)10000/t;
        line(i, 256 - (float)10000/t, i, 256 + (float)10000/t, &frame, blue);

        line(player_x, player_y, player_x+t*cos(trueA), player_y+t*sin(trueA), &debug, white);
    }

    sprintf(name, "frame%3.3i.tga", f);
    frame.write_tga_file(name);

    player_a += 1 * DEGREE_TO_RAD;
    f++;
    }


    debug.flip_vertically(); // i want to have the origin at the left bottom corner of the debug
    debug.write_tga_file("output.tga");
    //frame.flip_vertically();
    frame.write_tga_file("frame.tga");
    return 0;
}