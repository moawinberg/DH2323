#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDL2Auxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;
using glm::vec2;
using glm::abs;


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
void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result );
void DrawLineSDL( ivec2 a, ivec2 b, vec3 color );

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
int dx;
    int dy;
    SDL_GetRelativeMouseState(&dx, &dy);
}

// 2. It should take the 3D position of a vertex v and compute its 2D image position and store it in the given 2D integer vector p
void VertexShader( const vec3& v, ivec2& p ) {
     
// 1.4 Finally we assume that the camera can both rotate and translate. Then we can perform the transformation of a point from world coordinate system to camera coordinate system
    vec3 transformation = R * (v - cameraPos); // P' = (P - C) R (8)
    
// 1.1 Then the projection of an arbitrary 3D point (X, Y, Z) to a 2D point (x, y) in the camera image can be written as
    p.x = focalLength * (transformation.x / transformation.z) + (SCREEN_WIDTH/2); // x = f X/Z+W2 (3)
    p.y = focalLength * (transformation.y / transformation.z) + (SCREEN_HEIGHT/2);// y = f X/Z+W2 (4)

}

// 3. To draw lines in 2D you can use a function that does linear interpolation similar to what you wrote for the first lab. Instead of interpolating pixel colors represented by glm::vec3 we will interpolate pixel positions represented by glm::ivec2.
void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result ) {
    int N = result.size();
    vec2 step = vec2(b-a) / float(max(N-1,1));
    vec2 current( a );
    for( int i=0; i<N; ++i ) {
        result[i] = current;
        current += step;
    }
}


void DrawLineSDL(ivec2 a, ivec2 b, vec3 color ) {
// 3. If a and b represent the start and end of the line segment we can then compute the number of pixels to draw as:
    ivec2 delta = glm::abs( a - b );
    int pixels = glm::max( delta.x, delta.y ) + 1;
// 3. You can then get the pixel positions of the line by calling the Interpolation function:
    vector<ivec2> line( pixels );
    Interpolate( a, b, line );

// 3. When we have these we can loop through all of them and call PutPixelSDL for these pixel positions to draw the line.
    for (int i = 0; i < pixels; i++) {
        sdlAux->putPixel(line[i].x, line[i].y, color);
    }
}

// 3. Add another function to draw the edges of a triangle:
void DrawPolygonEdges( const vector<vec3>& vertices ) {
    int V = vertices.size();
// Transform each vertex from 3D world position to 2D image position:
    vector<ivec2> projectedVertices( V );
    
    for( int i=0; i<V; ++i ) {
        VertexShader( vertices[i], projectedVertices[i] );
    }
    
// Loop over all vertices and draw the edge from it to the next vertex:
    for( int i=0; i<V; ++i ) {
// 3. The % operator gives the remainder after division of two integers. We use it to make the next index after the last index wrap around to the first index.
        int j = (i+1)%V; // The next vertex
        vec3 color( 1, 1, 1 );
        DrawLineSDL( projectedVertices[i], projectedVertices[j], color );
    }
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
        DrawPolygonEdges( vertices );
    }
    sdlAux->render();
}

