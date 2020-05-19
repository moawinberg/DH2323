/*
 * SDL2 skeleton for lab assignments 1–3 of the KTH course DH2323,
 * Computer Graphics and Interaction (and also SU DA3001, Datalogi II).
 *
 * See README.md for details.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDL2auxiliary.h"

using namespace std;
using glm::vec3;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

vector<float> result( 10 );

vector<vec3> vec_result( 4 );
const vec3 vec_a(1,4,9.2);
const vec3 vec_b(4,1,9.8);

vector<vec3> stars( 1000 );
int t;
float v = 0.0006;

SDL2Aux *sdlAux;
SDL_Surface* screen;

// --------------------------------------------------------
// FUNCTION DECLARATIONS

void DrawColors();
void DrawStars();
void Update();
void InterpolateResult( float a, float b, vector<float>& result );
void Interpolate( vec3 a, vec3 b, vector<vec3>& result );

// --------------------------------------------------------
// FUNCTION DEFINITIONS

int main(int argc, char* argv[]) {
    sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    InterpolateResult( 5, 14, result );
    Interpolate( vec_a, vec_b, vec_result );
    t = SDL_GetTicks();
    for(int i = 0; i < 1000; i++) {
        stars[i].x =  2.0f * float(rand()) / float(RAND_MAX) - 1.0f; // -1 <= x <= 1
        stars[i].y = 2.0f * float(rand()) / float(RAND_MAX) - 1.0f; //  -1 <= y <= 1
        stars[i].z = float(rand()) / float(RAND_MAX); //  0 <= z <= 1
    }
    
    while (!sdlAux->quitEvent()) {
        DrawColors();
        Update();
        // DrawStars();
    }
    
    sdlAux->saveBMP("screenshot.bmp");
    return 0;
}

void Update() {
    int t2 = SDL_GetTicks(); // current time
    float dt = float(t2-t);
    t = t2;
    
    for (int s = 0; s < stars.size(); ++s) {
        stars[s].z = stars[s].z - v*dt;
        if (stars[s].z <= 0)
            stars[s].z += 1;
        if (stars[s].z > 1)
            stars[s].z -= 1;
    }
}

void DrawColors() {
    sdlAux->clearPixels();
    
    vec3 topLeft(1,0,0); // red
    vec3 topRight(0,0,1); // blue
    vec3 bottomLeft(0,1,0); // green
    vec3 bottomRight(1,1,0); // yellow
    
    // Interpolate left and right edges
    vector<vec3> leftSide(SCREEN_HEIGHT);
    vector<vec3> rightSide(SCREEN_HEIGHT);
    Interpolate(topLeft, bottomLeft, leftSide);
    Interpolate(topRight, bottomRight, rightSide);
    
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
       for (int x = 0; x < SCREEN_WIDTH; ++x) {
          vector<vec3> colors(SCREEN_WIDTH);
          Interpolate(leftSide[y],rightSide[y],colors);
          sdlAux->putPixel(x, y, colors[x]);
       }
   }

    sdlAux->render();
}

void DrawStars() {
    sdlAux->clearPixels();

    // fill screen with black pixels
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
      for (int x = 0; x < SCREEN_WIDTH; ++x) {
        vec3 color(0.0, 0.0, 0.0);
        sdlAux->putPixel(x, y, color);
      }
    }

    int f = SCREEN_HEIGHT / 2;
    for (size_t s = 0; s < stars.size(); s++) {
        // projection pinhole camera
        int u = f * (stars[s].x / stars[s].z) + (SCREEN_WIDTH / 2);
        int v = f * (stars[s].y / stars[s].z) + (SCREEN_HEIGHT / 2);

        vec3 color = 0.1f * vec3(1, 1, 1) / (stars[s].z * stars[s].z);

        sdlAux->putPixel(u, v, color);
    }

    sdlAux->render();
}

void InterpolateResult( float a, float b, vector<float>& result ) {
    int size = result.size();
    
    if (size == 1) {
        result[0] = (a + b) / 2;
        return;
    }

    for ( int i = 0; i < size; ++i ) {
        result[i] = a + i * (b - a) / (size - 1);
        cout << result[i] << " ";
    }
}

void Interpolate( vec3 a, vec3 b, vector<vec3>& result ) {
    int size = result.size();

    if (size == 1)
        return;

    for (int i = 0; i < result.size(); ++i) {
        result[i].x = a.x + i * ((b.x - a.x) / (size - 1));
        result[i].y = a.y + i * ((b.y - a.y) / (size - 1));
        result[i].z = a.z + i * ((b.z - a.z) / (size - 1));
    }
}


