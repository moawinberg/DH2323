//
//  main.cpp
//  project
//
//  Created by moa and isabella on 2020-05-05.
//
 
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
 
using namespace std;
using glm::vec3;
 
// --------------------------------------------------------
// GLOBAL VARIABLES
 
float zoom = -5.0;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
GLfloat angle = 0;
GLdouble radius = 0.3;
vector<vec3> stars( 1000 ); // code from lab 1 render track

// --------------------------------------------------------
// the below code is from https://www.lighthouse3d.com/tutorials/glut-tutorial/keyboard-example-moving-around-the-world/
float cameraAngle = 0.0; // angle of rotation for the camera direction
float lx=0.0f,lz=-1.0f; // actual vector representing the camera's direction
float x=0.0f,z=5.0f; // XZ position of the camera
// --------------------------------------------------------

 
// --------------------------------------------------------
// FUNCTION DECLARATIONS
 
void light();
void createSun();
void createPlanet();
void createStars();
void display(void);
void reshape(int x, int y);
void idle(void);
void processSpecialKeys(int key, int xx, int yy);
 
// --------------------------------------------------------
// FUNCTION DEFINITIONS
 
void light() {
    // clear color before set light source and material light
    glClearColor(0.0,0.0,0.0,0.0);
    
    // set custom values to ambient, diffuse and specular light
    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
 
    const GLfloat material_ambient[]    = { 0.5f, 0.5f, 0.5f, 1.0f };
    const GLfloat material_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat material_specular[]   = { 0.5f, 0.4f, 0, 0 };
    
    const GLfloat light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    // add a bit of shine
    const GLfloat shine[] = { 20.0f };
    
    // enable lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
 
    // light source
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    // material light
    glMaterialfv(GL_FRONT, GL_AMBIENT,   material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shine);
}
 
void createSun() {
    GLUquadric *quad;
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glColor3f(0.8, 0.2, 0.1);
    quad = gluNewQuadric();
    gluSphere(quad, radius, 100, 100);
    glPopMatrix();
}

void createPlanet() {
    GLUquadric *quad;

    glPushMatrix();
    glColor3f(0.1, 0.2, 0.8);
    
    glTranslatef(-radius, 0, -radius); //move away from origo
 
    // rotation in circular motion
    glTranslatef(radius, radius, radius);
    glRotatef(angle, 0, 1, 0);
    glTranslatef(-radius,-radius, -radius);
        
    quad = gluNewQuadric();
    gluSphere(quad, radius*0.2, 100, 100);
    glPopMatrix();
}
 
void createStars() {
    glPointSize(1.0f);
    for (size_t s = 0; s < stars.size(); s++) {
        glBegin(GL_POINTS);
        vec3 color = 0.1f * vec3(1, 1, 1) / (stars[s].z * stars[s].z);
        glColor3f(color.x,color.y,color.z);
        glVertex3f(stars[s].x, stars[s].y, stars[s].z);
        glEnd();
    }
}
 
// --------------------------------------------------------
// this function is from https://www.lighthouse3d.com/tutorials/glut-tutorial/keyboard-example-moving-around-the-world/
void processSpecialKeys(int key, int xx, int yy) {
    float fraction = 0.1f;
    switch (key) {
        case GLUT_KEY_LEFT :
            cameraAngle -= 0.01f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case GLUT_KEY_RIGHT :
            cameraAngle += 0.01f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case GLUT_KEY_UP :
            x += lx * fraction;
            z += lz * fraction;
            break;
        case GLUT_KEY_DOWN :
            x -= lx * fraction;
            z -= lz * fraction;
            break;
    }
}
// --------------------------------------------------------
 
void display(void) {
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the drawing buffer.
    glLoadIdentity(); // clear the identity matrix.
    
    // --------------------------------------------------------
    // this row is from https://www.lighthouse3d.com/tutorials/glut-tutorial/keyboard-example-moving-around-the-world/
    gluLookAt(x, 1.0f, z, x+lx, 1.0f, z+lz, 0.0f, 1.0f,  0.0f);
    // --------------------------------------------------------
 
    glTranslatef(0.0,0.0, zoom); // zoom view
    
    createSun();
    createPlanet();
    createStars();
 
    glFlush();
    glutSwapBuffers();
}
 
void reshape(int x, int y) {
    if (y == 0 || x == 0) return;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(39.0,(GLdouble)x/(GLdouble)y,0.6,21.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, x, y);
}
 
void idle(void) {
    angle ++; // update angle for earth rotation
    glutPostRedisplay();
}
 
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Small Solar System in c++");
    
    // --------------------------------------------------------
    // code from lab 1 render track
    for(int i = 0; i < 1000; i++) {
        stars[i].x =  2.0f * float(rand()) / float(RAND_MAX) - 1.0f; // -1 <= x <= 1
        stars[i].y = 2.0f * float(rand()) / float(RAND_MAX) - 1.0f; //  -1 <= y <= 1
        stars[i].z = float(rand()) / float(RAND_MAX); //  0 <= z <= 1
    }
    // --------------------------------------------------------
    
    light();
 
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
 
    return 0;
}
 
 
 
 
 
 

