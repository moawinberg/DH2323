#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDL2Auxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float focalLength = SCREEN_HEIGHT;

SDL2Aux *sdlAux;
int t;
vector<Triangle> triangles;

vec3 cameraPos( 0, 0, -3.001 );
mat3 R;
float yaw = 0; // Yaw angle controlling camera rotation around y-axis
vec3 lightPos(0, -0.5, -0.7);

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void Rotate();
void VertexShader( const vec3& v, ivec2& p );


int main( int argc, char* argv[] ) {
    LoadTestModel( triangles );
    sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
    t = SDL_GetTicks();    // Set start value for timer.

 while (!sdlAux->quitEvent()) {
     Update();
     Draw();
  }

    sdlAux->saveBMP("screenshot.bmp");
    return 0;
}

void Rotate() {
    R = mat3(cos(yaw), 0, sin(yaw), 0, 1, 0, -sin(yaw), 0, cos(yaw));
}

void Update() {
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float radians = 0.1;
        
    // Controls camera
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
    
    // Controls light
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

// 2. It should take the 3D position of a vertex v and compute its 2D image position and store it in the given 2D integer vector p
void VertexShader( const vec3& v, ivec2& p ) {
     
// 1.4 Finally we assume that the camera can both rotate and translate. Then we can perform the transformation of a point from world coordinate system to camera coordinate system
    vec3 transformation = R * (v - cameraPos); // P' = (P - C) R (8)
    
// 1.1 Then the projection of an arbitrary 3D point (X, Y, Z) to a 2D point (x, y) in the camera image can be written as
    p.x = focalLength * (transformation.x / transformation.z) + (SCREEN_WIDTH/2); // x = f X/Z+W2 (3)
    p.y = focalLength * (transformation.y / transformation.z) + (SCREEN_HEIGHT/2);// y = f X/Z+W2 (4)

}

// 2. The Draw function in the given skeleton program looks like:
void Draw() {
    sdlAux->clearPixels();

// 2. The function loops through all triangles and all vertices of the triangles and calls the function VertexShader on each.
    for( int i=0; i<triangles.size(); ++i ) {
        vector<vec3> vertices(3);

        vertices[0] = triangles[i].v0;
        vertices[1] = triangles[i].v1;
        vertices[2] = triangles[i].v2;
        for(int v=0; v<3; ++v) {
            ivec2 projPos;
            VertexShader( vertices[v], projPos );
            vec3 color(1,1,1);
            sdlAux->putPixel(projPos.x, projPos.y, color);
        }
    }
    sdlAux->render();
}


