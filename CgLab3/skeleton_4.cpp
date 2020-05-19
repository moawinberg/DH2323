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

vec3 currentColor;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void Rotate();
void VertexShader( const vec3& v, ivec2& p );
void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result );
void DrawLineSDL( ivec2 a, ivec2 b, vec3 color );
void ComputePolygonRows( const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels, vector<ivec2>& rightPixels );
void DrawPolygonRows( vector<ivec2>leftPixels, vector<ivec2>rightPixels );

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

void DrawLineSDL( ivec2 a, ivec2 b, vec3 color ) {
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

void ComputePolygonRows( const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels, vector<ivec2>& rightPixels ) {
    
// Find max and min y-value of the polygon and compute the number of rows it occupies.
    int max_y = glm::max(glm::max(vertexPixels[0].y, vertexPixels[1].y), vertexPixels[2].y);
    int min_y = glm::min(glm::min(vertexPixels[0].y, vertexPixels[1].y), vertexPixels[2].y);

// 4. As an example consider a triangle which has the projected vertices: (10,20), (30,10), (20,40), it should be drawn as 40-10+1=31 rows.
    int ROWS = max_y - min_y + 1;
    
// Resize leftPixels and rightPixels so that they have an element for each row.
    leftPixels.resize(ROWS);
    rightPixels.resize(ROWS);
       
// Initialize the x-coordinates in leftPixels to some really large value and the x-coordinates in rightPixels to some really small value.
// 4. One way to fill these arrays with values representing the polygon is to first initialize the start arrays with really big values and the end array with really small values:
    for( int i=0; i<ROWS; ++i ) {
        leftPixels[i].x = +numeric_limits<int>::max();
        rightPixels[i].x = -numeric_limits<int>::max();
        leftPixels[i].y = i + min_y;
        rightPixels[i].y = i + min_y;
    }
        
// Loop through all edges of the polygon and use linear interpolation to find the x-coordinate for each row it occupies. Update the corresponding values in rightPixels and leftPixels.
    for (int i = 0; i< vertexPixels.size() ; ++i) {

// First vertex
        ivec2 p1 = vertexPixels[i];
// Next vertex
        ivec2 p2 = vertexPixels[(i + 1) % vertexPixels.size()];
// p1 and p2 start and end of the line segment we can then compute the number of pixels to draw as:
        ivec2 delta = abs(p2 - p1);
        int pixels = max(delta.x, delta.y) + 1;
// You can then get the pixel positions of the line by calling the Interpolation function
        vector<ivec2> line(pixels);
        Interpolate(p1, p2, line);
// Loop through every line (edge)
    for (int j = 0; j < line.size(); j++) {
        for (int k = 0; k < ROWS; k++) {
            if (leftPixels[k].y == line[j].y) {
                if (leftPixels[k].x>line[j].x)
                    leftPixels[k].x = line[j].x;
                if (rightPixels[k].x<line[j].x)
                    rightPixels[k].x = line[j].x;
            break;
            }
        }
    }
    }
}

// 4. When you got this working you can write a function that draws the computed rows:
void DrawPolygonRows(vector<ivec2>leftPixels, vector<ivec2>rightPixels) {
// 4. This function should call PutPixelSDL for each pixel between the start and end for each row.
    for (int i = 0; i < leftPixels.size(); i++) {
        DrawLineSDL(leftPixels[i], rightPixels[i], currentColor);
    }
}

void DrawPolygon( const vector<vec3>& vertices ) {
    int V = vertices.size();
    vector<ivec2> vertexPixels( V );
    
    for( int i=0; i<V; ++i )
        VertexShader( vertices[i], vertexPixels[i] );
    vector<ivec2> leftPixels;
    vector<ivec2> rightPixels;
    ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
    DrawPolygonRows( leftPixels, rightPixels );
}

void Draw() {
    sdlAux->clearPixels();

    for( int i=0; i<triangles.size(); ++i ) {
// To signal what color to draw you can create a new global variable:
        currentColor = triangles[i].color;
        vector<vec3> vertices(3);
        vertices[0] = triangles[i].v0;
        vertices[1] = triangles[i].v1;
        vertices[2] = triangles[i].v2;
        DrawPolygon( vertices );
    }
    sdlAux->render();
}

