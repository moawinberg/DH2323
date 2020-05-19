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
using glm::distance;
using glm::normalize;
using glm::dot;
 
 
// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
 
float pi = 3.14159265358979323846;
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float focalLength = SCREEN_HEIGHT;
 
SDL2Aux *sdlAux;
int t;
vector<Triangle> triangles;
 
vec3 cameraPos( 0, 0, -3.001 );
mat3 R;
float yaw = 0; // Yaw angle controlling camera rotation around y-axis
 
vec3 currentColor;
 
float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
 
// from 6.1 Per Vertex Illumination
vec3 lightPos(0,-0.5,-0.7);
vec3 lightPower = 10.1f*vec3( 1, 1, 1 ); // increased light from 1 to 10
vec3 indirectLightPowerPerArea = 0.5f*vec3( 1, 1, 1 );
 
// 6.2 Per Pixel Illumination
vec3 currentNormal;
vec3 currentReflectance;
 
// --------------------------------------------------------
// STRUCTS
 
struct Pixel {
    int x;
    int y;
    float zinv;
    vec3 illumination;
    vec3 pos3d;
};
 
struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 reflectance;
};
 
 
// ----------------------------------------------------------------------------
// FUNCTIONS
 
void Update();
void Draw();
void Rotate();
void VertexShader( const Vertex& v, Pixel& p );
void Interpolate( Pixel a, Pixel b, vector<Pixel>& result );
void DrawLineSDL( Pixel a, Pixel b, vec3 color );
void ComputePolygonRows( const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels, vector<Pixel>& rightPixels );
void DrawPolygonRows( vector<Pixel>leftPixels, vector<Pixel>rightPixels );
void PixelShader( const Pixel& p );
 
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
 
void PixelShader( const Pixel& p ) {
    int x = p.x;
    int y = p.y;
    if( p.zinv > depthBuffer[y][x] ) {
        depthBuffer[y][x] = p.zinv;
        
        //  Per Pixel Illumination
        vec3 dir = normalize(lightPos - p.pos3d);
        float r = distance(lightPos, p.pos3d);
        
        float A = 4*pi*r*r;
           
        float angle = dot(dir, currentNormal);
        if (angle < 0) angle = 0;
        
        vec3 D = lightPower * angle / A;
        vec3 total_reflectance = currentReflectance * (D + indirectLightPowerPerArea);
        
        sdlAux->putPixel(x, y,  total_reflectance); // currentColor / p.illumination / total_reflectance
    }
}
 
// 2. It should take the 3D position of a vertex v and compute its 2D image position and store it in the given 2D integer vector p
void VertexShader( const Vertex& v, Pixel& p ) {
     
    // 1.4 Finally we assume that the camera can both rotate and translate. Then we can perform the transformation of a point from world coordinate system to camera coordinate system
    vec3 transformation = R * (v.position - cameraPos); // P' = (P - C) R (8)
    
    p.zinv = 1.0f / transformation.z;
    
    // 1.1 Then the projection of an arbitrary 3D point (X, Y, Z) to a 2D point (x, y) in the camera image can be written as
    p.x = focalLength * (transformation.x / transformation.z) + (SCREEN_WIDTH/2); // x = f X/Z+W2 (3)
    p.y = focalLength * (transformation.y / transformation.z) + (SCREEN_HEIGHT/2);// y = f X/Z+W2 (4)
    p.pos3d = v.position;
    
    //  Per Vertex Illumination
    vec3 dir = normalize(lightPos - p.pos3d);
    float r = distance(lightPos, p.pos3d);
    
    float A = 4*pi*r*r;
       
    float angle = dot(dir, currentNormal);
    if (angle < 0) angle = 0;
    
    vec3 D = lightPower * angle / A;
    p.illumination = currentReflectance * (D + indirectLightPowerPerArea);
}
 
 
// 3. To draw lines in 2D you can use a function that does linear interpolation similar to what you wrote for the first lab. Instead of interpolating pixel colors represented by glm::vec3 we will interpolate pixel positions represented by glm::ivec2.
void Interpolate( Pixel a, Pixel b, vector<Pixel>& result ) {
    int N = result.size();
    
    float step_x = (b.x - a.x) / (float)(glm::max(N - 1, 1));
    float step_y = (b.y - a.y) / (float)(glm::max(N - 1, 1));
    float step_z = (b.zinv - a.zinv) / (float)(glm::max(N - 1, 1));
 
    vec3 step_pos3d = (b.pos3d - a.pos3d) / (float)(glm::max(N - 1, 1)); // 6.1/6.2
    vec3 step_illumination = (b.illumination - a.illumination) / (float)(glm::max(N - 1, 1));
    
    Pixel current( a );
    for( int i=0; i<N; ++i ) {
        current.x = a.x + i * step_x;
        current.y = a.y + i * step_y;
        current.zinv = a.zinv + i * step_z;
        current.illumination += step_illumination;
        current.pos3d += step_pos3d;
 
        result[i] = current;
    }
}
 
 
void DrawLineSDL( Pixel a, Pixel b, vec3 color ) {
    // 3. If a and b represent the start and end of the line segment we can then compute the number of pixels to draw as:
    Pixel delta;
    delta.x = abs(a.x - b.x);
    delta.y = abs(a.y - b.y);
    int pixels = glm::max( delta.x, delta.y ) + 1;
    // 3. You can then get the pixel positions of the line by calling the Interpolation function:
    vector<Pixel> line( pixels );
    Interpolate( a, b, line );
    // 3. When we have these we can loop through all of them and call PutPixelSDL for these pixel positions to draw the line.
    for (int i = 0; i < pixels; i++) {
        PixelShader(line[i]); // from 6 illumination
    }
}
 
// 3. Add another function to draw the edges of a triangle:
void DrawPolygonEdges( const vector<Vertex>& vertices ) {
    int V = vertices.size();
    // Transform each vertex from 3D world position to 2D image position:
    vector<Pixel> projectedVertices( V );
    
    for( int i=0; i<V; ++i ) {
        VertexShader( vertices[i], projectedVertices[i] );
    }
    
// Loop over all vertices and draw the edge from it to the next vertex:
    for( int i=0; i<V; ++i ) {
        // 3. The % operator gives the remainder after division of two integers. We use it to make the next index after the last index wrap around to the first index.
        int j = (i+1)%V; // The next vertex
        vec3 color( 1, 1, 1 );
        DrawLineSDL( projectedVertices[i], projectedVertices[j], currentColor );
    }
}
 
void ComputePolygonRows( const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels, vector<Pixel>& rightPixels ) {
    
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
        Pixel p1 = vertexPixels[i];
        // Next vertex
        Pixel p2 = vertexPixels[(i + 1) % vertexPixels.size()];
        // p1 and p2 start and end of the line segment we can then compute the number of pixels to draw as:
        Pixel delta;
        delta.x = abs(p2.x - p1.x);
        delta.y = abs(p2.y - p1.y);
        int pixels = max(delta.x, delta.y) + 1;
        // You can then get the pixel positions of the line by calling the Interpolation function
        vector<Pixel> line(pixels);
        Interpolate(p1, p2, line);
    
        for (Pixel p : line) {
            int i = p.y - min_y;
            if (p.x < leftPixels[i].x) {
                leftPixels[i].x = p.x;
                leftPixels[i].zinv = p.zinv;
                leftPixels[i].illumination = p.illumination;
                leftPixels[i].pos3d = p.pos3d;
            }
            if (p.x >= rightPixels[i].x) {
                rightPixels[i].x = p.x;
                rightPixels[i].zinv = p.zinv;
                rightPixels[i].illumination = p.illumination;
                rightPixels[i].pos3d = p.pos3d;
            }
        }
    }
}

 
// 4. When you got this working you can write a function that draws the computed rows:
void DrawPolygonRows(vector<Pixel>leftPixels, vector<Pixel>rightPixels) {
    // 4. This function should call PutPixelSDL for each pixel between the start and end for each row.
    for (int i = 0; i < leftPixels.size(); i++) {
        DrawLineSDL(leftPixels[i], rightPixels[i], currentColor);
    }
}
 
void DrawPolygon( const vector<Vertex>& vertices ) {
    int V = vertices.size();
    vector<Pixel> vertexPixels( V );
    
    for( int i=0; i<V; ++i )
        VertexShader( vertices[i], vertexPixels[i] );
    vector<Pixel> leftPixels;
    vector<Pixel> rightPixels;
    ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
    DrawPolygonRows( leftPixels, rightPixels );
}
 
void Draw() {
    sdlAux->clearPixels();
    
    for( int y=0; y<SCREEN_HEIGHT; ++y )
        for( int x=0; x<SCREEN_WIDTH; ++x )
            depthBuffer[y][x] = 0;
 
    for( int i=0; i<triangles.size(); ++i ) {
        // To signal what color to draw you can create a new global variable:
        vector<Vertex> vertices(3);
        vertices[0].position = triangles[i].v0;
        vertices[1].position = triangles[i].v1;
        vertices[2].position = triangles[i].v2;
        
        currentColor = triangles[i].color;
        currentNormal = triangles[i].normal;
        currentReflectance = triangles[i].color;
 
        DrawPolygon( vertices );
    }
    sdlAux->render();
}
 

