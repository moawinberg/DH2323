/*
 * SDL2 skeleton for lab assignments 1–3 of the KTH course DH2323,
 * Computer Graphics and Interaction (and also SU DA3001, Datalogi II).
 *
 * See README.md for details.
 */
 
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDL2Auxiliary.h"
#include "TestModel.h"
 
using namespace std;
using glm::vec3;
using glm::mat3;
using glm::dot;
using glm::distance;
using glm::normalize;
using glm::inverse;
 
// --------------------------------------------------------
// GLOBAL VARIABLES
 
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// for rotation
//const int SCREEN_WIDTH = 100;
//const int SCREEN_HEIGHT = 100;
SDL2Aux *sdlAux;
vector<Triangle> triangles;
 
float focalLength = SCREEN_HEIGHT;
vec3 cameraPos(0, 0, -3); // negative z-value for upside down, -3 to display all
 
mat3 R; // rotation matrix
float yaw; // angle which the camera should be rotated around the y-axis
 
vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3(1,1,1);
float pi = 3.14159265358979323846;
vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
 
// --------------------------------------------------------
// FUNCTION DECLARATIONS
 
struct Intersection
{
    vec3 position;
    float distance;
    int triangleIndex;
};

bool ClosestIntersection(vec3 start,
                         vec3 dir,
                         const vector<Triangle>& triangles,
                         Intersection& closestIntersection);
void Draw();
void Update();
void Rotate();
vec3 DirectLight( const Intersection& i);
 
// --------------------------------------------------------
// FUNCTION DEFINITIONS
int main(int argc, char* argv[]) {
    sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
    LoadTestModel(triangles);
    while (!sdlAux->quitEvent()) {
        Update();
        Draw();
    }
 
    sdlAux->saveBMP("screenshot.bmp");
    return 0;
}
 
void Draw() {
    sdlAux->clearPixels();
 
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            vec3 direction((x-SCREEN_WIDTH/2), (y-SCREEN_HEIGHT/2), focalLength);
            Intersection intersection;
            if (ClosestIntersection(cameraPos, direction, triangles, intersection)) {
                vec3 light = DirectLight(intersection);
                vec3 color = triangles[intersection.triangleIndex].color;
                // vec3 colorWithLight = color * light;
                vec3 colorWithLightAndIndirectLight = color * (light + indirectLight);
                sdlAux->putPixel(x, y, colorWithLightAndIndirectLight);
            } else {
                vec3 color(0.0, 0.0, 0.0);
                sdlAux->putPixel(x, y, color);
            }
        }
    }
 
    sdlAux->render();
}
void Rotate() {
    R = mat3(cos(yaw), 0, sin(yaw), 0, 1, 0, -sin(yaw), 0, cos(yaw));
}

void Update()
    {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float radians = 0.1;
        
    // controls camera
    if (state[SDL_SCANCODE_UP]) {
        cameraPos.z += radians;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        cameraPos.z -= radians;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        yaw += radians;
        Rotate();
    }
    if (state[SDL_SCANCODE_LEFT]) {
        yaw -= radians;
        Rotate();
    }
        
    // controls light
    if (state[SDL_SCANCODE_W] ) {
        lightPos.z += radians;
    }
    if (state[SDL_SCANCODE_S] ) {
        lightPos.z -= radians;
    }
    if (state[SDL_SCANCODE_A] ) {
        lightPos.x -= radians;
    }
    if (state[SDL_SCANCODE_D] ) {
        lightPos.x += radians;
    }
    if (state[SDL_SCANCODE_Q] ) {
        lightPos.y += radians;
    }
    if (state[SDL_SCANCODE_E] ) {
        lightPos.y -= radians;
    }
}
 
bool ClosestIntersection(vec3 start,
                         vec3 dir,
                         const vector<Triangle>& triangles,
                         Intersection& closestIntersection) {
    closestIntersection.distance = std::numeric_limits<float>::max();
    bool does_intersect = false;
    
    for (int i = 0; i < triangles.size(); i++) {
        Triangle triangle = triangles[i]; // take out the indexed triangle
        
        // set variables
        vec3 v0 = triangle.v0*R;
        vec3 v1 = triangle.v1*R;
        vec3 v2 = triangle.v2*R;
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 b = start - v0;
        mat3 A( -dir, e1, e2 );
        vec3 x = inverse( A ) * b;
        
        // triangle values
        float t = x.x;
        float u = x.y;
        float v = x.z;
        
        if (u >= 0 && v >= 0 && u+v <= 1) {
            if (t > 0 && t < closestIntersection.distance) {
                closestIntersection.distance = t;
                closestIntersection.position = start + (dir * closestIntersection.distance);
                closestIntersection.triangleIndex = i;
            }
            does_intersect = true;
        }
    }
    return does_intersect;
}
 
vec3 DirectLight( const Intersection& i) {
    vec3 normal = triangles[i.triangleIndex].normal;
    vec3 dir = normalize(lightPos - i.position);
    float r = distance(lightPos, i.position);
    
    float A = 4*pi*r*r;
    
    vec3 B = lightColor/A;
       
    float angle = dot(dir, normal);
    if (angle < 0) angle = 0;
       
    // check if in shadow with closestIntersection
    Intersection intersection_shadow;
    if (ClosestIntersection(lightPos, -dir, triangles, intersection_shadow)) {
        if (intersection_shadow.distance < r - 0.0001f) {
            return vec3(0, 0, 0); // in shadow, return black
        }
    }
    
    vec3 D = B * angle;
    return D;
}

