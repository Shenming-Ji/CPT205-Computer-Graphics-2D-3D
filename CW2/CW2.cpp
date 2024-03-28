#define FREEGLUT_STATIC
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define GL_PI 3.1415f
#include "vector"
using namespace std;
float fltZoom = 1.0f;                                            // Zoom amount
float fltX0 = 0.0f;                                              // Camera position (0, 0, 5)
float fltY0 = 0.0f;
float fltZ0 = 5.0f;
float fltXRef = 0.0f;                                            // Look At reference point (0, 0, -20)
float fltYRef = 0.0f;
float fltZRef = -20.0f;
float fltXUp = 0.0f;                                             // Up vector  (0, 1, 0)
float fltYUp = 1.0f; 
float fltZUp = 0.0f;
float angle = 0.0f, rotationSpeed = 0.1f;                        // Star rotation angle
float side_angle = 0.0f, sideSpeed = 0.1f;                       // Star another angle
float x_z_angle = 0.0f, x_z_Speed = 0.05f;                       // planet rotation angle          
float e_angle = 0.0f, e_Speed = 0.1f;                            // planet another angle 
float telescope_angle = 10.0f;                                   // Telescope rotation angle
float len_angle = 25.0f, len_step = 0.0f;                        // Len of telescope rotation angle
float cradle_angle = 0.0f, cradle_step = 0.0f;                   // Cradle rotation angle
float body_rotate = 195.0f, body_move = 0.0f, leg_swag = 0.0f;   // Stargazer rotation and movements 
float propeller_rotation = 45.0f, drone_rotation = 40.0f;        // Drone rotation 
float drone_x = -9.5f, drone_y = 1.0f;                           // Drone movements 
float planet_angle = 45.0f;                                      // Blue planet rotation
int windowWidth = 1920, windowHeight = 1080;                     // Initial window size
GLboolean shell = GL_FALSE;                                      // Menu change the shell
GLboolean arrive = GL_FALSE;                                     // Verify the leg and arm
GLboolean Wire = GL_FALSE;                                       // Star default setting
GLint imagewidth0, imagewidth1, imagewidth2, imagewidth3;        // texture mapping
GLint imageheight0, imageheight1, imageheight2, imageheight3;
GLint pixellength0, pixellength1, pixellength2, pixellength3;
vector<GLubyte*>p; 
GLuint texture[4]; 
typedef enum {
    ICOSAHEDRON = 0,
    CONE,
    CUBE,
    TORUS,
    DODECAHEDRON,
    TEAPOT,
    OCTAHEDRON,
    TETRAHEDRON,
    SPHERE
}model;                                      // Menu Setting
GLint Centre = ICOSAHEDRON;                                    

// Image reading function
void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength) {
    GLubyte* pixeldata;
    FILE* pfile;
    fopen_s(&pfile, path, "rb");
    if (pfile == 0) exit(0);
    fseek(pfile, 0x0012, SEEK_SET);
    fread(&imagewidth, sizeof(imagewidth), 1, pfile);
    fread(&imageheight, sizeof(imageheight), 1, pfile);
    pixellength = imagewidth * 3;
    while (pixellength % 4 != 0)pixellength++;
    pixellength *= imageheight;
    pixeldata = (GLubyte*)malloc(pixellength);
    if (pixeldata == 0) exit(0);
    fseek(pfile, 54, SEEK_SET);
    fread(pixeldata, pixellength, 1, pfile);
    p.push_back(pixeldata); 
    fclose(pfile);
}

// Initial light setting
void light_init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHT0);

    // Light position
    GLfloat lightPosition[] = { 5.0f, 5.0f, 5.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Light properties
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Material
    GLfloat materialAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat materialDiffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat materialSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat materialShininess[] = { 10.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
    //glEnable(GL_COLOR_MATERIAL);
}

// Initial texture setting
void texture_init() {
    // set texture mapping
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    ReadImage("surface.bmp", imagewidth0, imageheight0, pixellength0);
    ReadImage("c16.bmp", imagewidth1, imageheight1, pixellength1);
    ReadImage("cloth.bmp", imagewidth2, imageheight2, pixellength2);
    ReadImage("surface.bmp", imagewidth3, imageheight3, pixellength3);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // set pixel storage modes (in the memory)
    glGenTextures(4, &texture[0]); // number of texture names to be generated and an array of texture names
    glBindTexture(GL_TEXTURE_2D, texture[0]); // target to which texture is bound and name of a texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth0, imageheight0, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth1, imageheight1, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth2, imageheight2, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[2]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth3, imageheight3, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[3]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

// Stargazer light setting
void light_Stargazer() {
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);  // Assuming you want to use light source GL_LIGHT0

    // Set up light properties
    GLfloat light_position[] = { -10.0, 10.0, 10.0, 1.0 };  // Example light position
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

    // Set up material properties
    GLfloat material_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat material_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat shininess = 50.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Drone light setting
void light_drone() {
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    // Set material properties
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Set light properties
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
}

// Construct a circle
void drawCircle(float radius, GLfloat centerX, GLfloat centerY, GLfloat centerZ) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 1000; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(1000);
        float x = centerX + radius * cos(theta);
        float z = centerZ + radius * sin(theta);
        glVertex3f(x, centerY, z);
    }
    glEnd();
}

// Menu Setting
void processMenu(int value) {
    switch (value) {
    case 1:
        Centre = ICOSAHEDRON;
        break;
    case 2:
        Centre = CUBE;
        break;
    case 3:
        Centre = DODECAHEDRON;
        break;
    case 4:
        Centre = OCTAHEDRON;
        break;
    case 5:
        Centre = TETRAHEDRON;
        break;
    case 6:
        Centre = SPHERE;
        break;
    case 7:
        Wire = GL_TRUE;
        break;
    case 8:
        Wire = GL_FALSE;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Construct a drone
void drone() {
    light_init();
    // construct drone
    glPushMatrix();
    glTranslatef(drone_x, drone_y, 0);
    glRotatef(30, 1, 0, 0);
    glRotatef(drone_rotation, 0, 1, 0);
    // body
    glColor3f(1, 1, 1);
    glPushMatrix();
    glScalef(0.7,0.5,1.5);
    glColor3f(0.5, 0.5, 0.5);
    glutSolidCube(0.7);
    glPopMatrix();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    light_drone();
    // rack
    // 1st
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    glutSolidCylinder(0.08, 1, 50, 50);
    glPopMatrix();
    // 2nd
    glPushMatrix();
    glRotatef(-45, 0, 1, 0);
    glutSolidCylinder(0.08, 1, 50, 50);
    glPopMatrix();
    // 3rd
    glPushMatrix();
    glRotatef(135, 0, 1, 0);
    glutSolidCylinder(0.08, 1, 50, 50);
    glPopMatrix();
    // 4th
    glPushMatrix();
    glRotatef(-135, 0, 1, 0);
    glutSolidCylinder(0.08, 1, 50, 50);
    glPopMatrix();
    // vertical rack
    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();
    glTranslatef(sqrt(2) / 2, 0.15, sqrt(2) / 2);
    glRotatef(90, 1, 0, 0);
    glutSolidCylinder(0.08, 0.3, 50, 50);
    glPopMatrix();
    // 2nd
    glPushMatrix();
    glTranslatef(-sqrt(2) / 2, 0.15, sqrt(2) / 2);
    glRotatef(90, 1, 0, 0);
    glutSolidCylinder(0.08, 0.3, 50, 50);
    glPopMatrix();
    // 3rd
    glPushMatrix();
    glTranslatef(-sqrt(2) / 2, 0.15, -sqrt(2) / 2);
    glRotatef(90, 1, 0, 0);
    glutSolidCylinder(0.08, 0.3, 50, 50);
    glPopMatrix();
    // 4th
    glPushMatrix();
    glTranslatef(sqrt(2) / 2, 0.15, -sqrt(2) / 2);
    glRotatef(90, 1, 0, 0);
    glutSolidCylinder(0.08, 0.3, 50, 50);
    glPopMatrix();
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    light_init();
    // propeller
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(sqrt(2) / 2, 0, sqrt(2) / 2);
    glRotatef(propeller_rotation, 0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glEnd();
    glPopMatrix();
    // 2nd
    glPushMatrix();
    glTranslatef(-sqrt(2) / 2, 0, sqrt(2) / 2);
    glRotatef(propeller_rotation, 0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glEnd();
    glPopMatrix();
    // 3rd
    glPushMatrix();
    glTranslatef(-sqrt(2) / 2, 0, -sqrt(2) / 2);
    glRotatef(propeller_rotation, 0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glEnd();
    glPopMatrix();
    // 4th
    glPushMatrix();
    glTranslatef(sqrt(2) / 2, 0, -sqrt(2) / 2);
    glRotatef(propeller_rotation, 0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(0.6 * (sqrt(2) / 2) / 2, 0.15, -0.6 * (sqrt(2) / 2) / 2);
    glVertex3f(-0.6 * (sqrt(2) / 2) / 2, 0.15, 0.6 * (sqrt(2) / 2) / 2);
    glEnd();
    glPopMatrix();
    // end
    glPopMatrix();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

// Construct a planet
void planet() {
    glPushMatrix();
    glTranslatef(-7, 5.6, -3);
    glPushMatrix();
    glRotatef(20, 1, 0, 0);
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glRotatef(planet_angle, 1, 0, 0);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    GLUquadric* quadricObj = gluNewQuadric(); // Create a quadric surface object
    gluQuadricTexture(quadricObj, GL_TRUE); // Set texture mode to true
    gluSphere(quadricObj, 1, 100, 100);
    gluDeleteQuadric(quadricObj);
    glPopMatrix();
    // orbit
    glColor3f(0.7, 0.9, 1.0);
    glPushMatrix();
    glRotatef(planet_angle, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    for (float i = 1.3; i < 1.7; i += 0.08) {
        drawCircle(i, 0, 0, 0);
    }
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

// Construct a stargazer
void Stargazer() {
    light_Stargazer();
    glPushMatrix();
    glTranslatef(-11.22, -4, 2);
    glPushMatrix();
    glTranslatef(body_move, 0, 0);
    glRotatef(body_rotate, 0, 1, 0);
    glPushMatrix();
    // body
    glColor3f(0.5, 0.5, 0.5);
    // front
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    //  left
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    glPopMatrix();
    // right
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    glPopMatrix();
    // back
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    glPopMatrix();
    // top
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    glPopMatrix();
    // bottom
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.9, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.6, 0.9, 0.6);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.6, -0.9, 0.6);
    glEnd();
    glPopMatrix();
    // left arm
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(0.6, 0.6, 0.6);
    glPushMatrix();
    glTranslatef(-0.8, 0.25, 0);
    glTranslatef(0, 0.6, -0);
    glRotatef(leg_swag, 1, 0, 0);
    glTranslatef(0, -0.6, -0);
    glScalef(0.1, 0.6, 0.1);
    glutSolidCube(2);
    glPopMatrix();

    // right arm
    glPushMatrix();
    glTranslatef(0.8, 0.25, 0);
    glTranslatef(0, 0.6, -0);
    glRotatef(-leg_swag, 1, 0, 0);
    glTranslatef(0, -0.6, -0);
    glScalef(0.1, 0.6, 0.1);
    glutSolidCube(2);
    glPopMatrix();

    // head
    glPushMatrix();
    glTranslatef(0, 1.2, 0);
    glutSolidCube(0.9);
    glPopMatrix();

    //left leg
    glPushMatrix();
    glTranslatef(-0.3, -1, 0);
    glTranslatef(0, 0.8, 0);
    glRotatef(-leg_swag, 1, 0, 0);
    glTranslatef(0, -0.8, 0);
    glScalef(0.2, 0.8, 0.2);
    glutSolidCube(2);
    glPopMatrix();

    // right leg
    glPushMatrix();
    glTranslatef(0.3, -1, 0);
    glTranslatef(0, 0.8, 0);
    glRotatef(leg_swag, 1, 0, 0);
    glTranslatef(0, -0.8, 0);
    glScalef(0.2, 0.8, 0.2);
    glutSolidCube(2);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHTING);
}

// Construct a telescope
void telescope() {
    // connection
    light_init();
    glColor3f(0.1, 0.1, 0.1);
    glPushMatrix();
    glRotatef(telescope_angle, 0, 1, 0);
    glScalef(1.3, 1.3, 1.3);
    glTranslatef(-7.5, -3.5, 2);
    glutSolidSphere(0.2, 50, 50);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    light_drone();
    // rack
    glColor3f(0.6, 0.6, 0.6);
    //1st
    glPushMatrix();
    glRotatef(15, 0, 0, 1);
    glPushMatrix();
    glRotatef(120, 1, 0, 0);
    glutSolidCylinder(0.07, 1.7, 100, 100);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    light_init();
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1);
    glTranslatef(0, 0, 0.7);
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glTranslatef(0, 0, 0.9);
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glColor3f(0.6, 0.6, 0.6);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    // 2nd
    glPushMatrix();
    glRotatef(15, 0, 0, 1);
    glPushMatrix();
    glRotatef(60, 1, 0, 0);
    light_drone();
    glutSolidCylinder(0.07, 1.7, 100, 100);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1);
    glTranslatef(0, 0, 0.7);
    light_init();
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glTranslatef(0, 0, 0.9);
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glColor3f(0.6, 0.6, 0.6);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    // 3rd
    glPushMatrix();
    glTranslatef(0.1, 0.1, 0);
    glRotatef(60, 0, 0, 1);
    glRotatef(-90, 0, 1, 0);
    glPushMatrix();
    light_drone();
    glutSolidCylinder(0.07, 1.7, 100, 100);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1);
    glTranslatef(0, 0, 0.7);
    light_init();
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glTranslatef(0, 0, 0.9);
    glutSolidCylinder(0.1, 0.2, 100, 100);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glColor3f(0.6, 0.6, 0.6);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    // len
    glPushMatrix();
    glTranslatef(0, -0.05, 0);
    glRotatef(cradle_angle + cradle_step, 0, 1, 0);
    glRotatef(len_angle, 0, 0, 1);
    glTranslatef(0, 0.05, 0);
    glPushMatrix();
    glTranslatef(-0.7, 0.05, 0);
    glRotatef(90, 0, 1, 0);
    light_drone();
    glutSolidCylinder(0.1, 2, 100, 100);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    glColor3f(0.1, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0, 0, 2);
    light_init();
    glutSolidCylinder(0.13, 0.2, 100, 100);
    glTranslatef(0, 0, -2);
    glutSolidCylinder(0.13, 0.2, 100, 100);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glColor3f(0.6, 0.6, 0.6);
    glPopMatrix();
    glPopMatrix();
    // translate matrix for all
    glPopMatrix();
    glPopMatrix();
}

// Construct a star
void star() {
    // center
    glPushMatrix();
    glRotatef(side_angle, 0, 0, -5);
    glPushMatrix();
    glRotatef(angle, 0.5f, 1.0f, 0.0f);
    glScalef(0.6, 0.6, 0.6);
    switch (Centre) {
    case ICOSAHEDRON:
        glScalef(1.0f, 1.0f, 1.0f);
        if (Wire) {
            glutWireIcosahedron();
        }
        else {
            glutSolidIcosahedron();
        }
        break;
    case CUBE:
        if (Wire) {
            glutWireCube(1.5f);
        }
        else {
            glutSolidCube(1.5f);
        }
        break;
    case DODECAHEDRON:
        glScalef(1, 1, 1);
        if (Wire) {
            glutWireDodecahedron();
        }
        else {
            glutSolidDodecahedron();
        }
        break;
    case OCTAHEDRON:
        glScalef(1.0f, 1.0f, 1.0f);
        if (Wire) {
            glutWireOctahedron();
        }
        else {
            glutSolidOctahedron();
        }
        break;
    case TETRAHEDRON:
        glScalef(1.2f, 1.2f, 1.2f);
        if (Wire) {
            glutWireTetrahedron();
        }
        else {
            glutSolidTetrahedron();
        }
        break;
    case SPHERE:
        if (Wire) {
            glutWireSphere(1, 50, 50);
        }
        else {
            glutSolidSphere(1, 50, 50);
        }
        break;
    default:
        break;
    }
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    // external structure
    glColor3f(1, 1, 1);
    // drawing 3 circles
    glLineWidth(2);
    // 1st layer
    // 1st
    glPushMatrix();
    glRotatef(x_z_angle, 1, 0, 0);
    glPushMatrix();
    drawCircle(2, 0, 0, 0);
    glRotatef(e_angle, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.6, 0.8, 1.0);
    glTranslatef(2, 0, 0);
    glutSolidSphere(0.1, 50, 10);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    // 2nd
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(x_z_angle, 0, 1, 1);
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    drawCircle(2, 0, 0, 0);
    glRotatef(-e_angle, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.6, 0.8, 1.0);
    glTranslatef(2, 0, 0);
    glutSolidSphere(0.1, 50, 10);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    //3rd
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(x_z_angle, 0, 1, 1);
    glPushMatrix();
    glRotatef(-45, 0, 0, 1);
    drawCircle(2, 0, 0, 0);
    glRotatef(e_angle, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.6, 0.8, 1.0);
    glTranslatef(-2, 0, 0);
    glutSolidSphere(0.1, 50, 10);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    // 2nd layer
    // 1st
    glColor3f(1, 1, 1);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(-x_z_angle, 0, 1, 1);
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    drawCircle(3.5, 0, 0, 0);
    glRotatef(-e_angle, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.6, 0.8, 1.0);
    glTranslatef(3.5, 0, 0);
    glutSolidSphere(0.1, 50, 10);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    //2nd
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(-x_z_angle, 0, 1, 1);
    glPushMatrix();
    glRotatef(0, 0, 0, 1);
    drawCircle(3.5, 0, 0, 0);

    glRotatef(-e_angle, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.6, 0.8, 1.0);
    glTranslatef(-3.5, 0, 0);
    glutSolidSphere(0.1, 50, 10);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHT0);
}

void display() {
    glClearColor(0.15, 0.15, 0.15, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    light_init();
    // bg
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-14, -7, -20);
    glTexCoord2f(0.0, 1.0); glVertex3f(-14, 7, -20);
    glTexCoord2f(1.0, 1.0); glVertex3f(14, 7, -20);
    glTexCoord2f(1.0, 0.0); glVertex3f(14, -7, -20);
    glEnd();
    gluLookAt(fltX0 * fltZoom, fltY0 * fltZoom, fltZ0 * fltZoom, fltXRef, fltYRef, fltZRef, fltXUp,
        fltYUp, fltZUp);
    // model
    glPushMatrix();
    glTranslatef(8, 2.7, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    star();
    glPopMatrix();
    // Telescope
    glBindTexture(GL_TEXTURE_2D, 0);
    telescope();
    // Human
    glBindTexture(GL_TEXTURE_2D, 0);
    Stargazer();
    // DJI drone :)
    glBindTexture(GL_TEXTURE_2D, 0);
    drone();
    // planet
    glBindTexture(GL_TEXTURE_2D, 0);
    planet();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    GLfloat ratio;
    GLfloat coordinatesize = 7.0f;  // -7,7,-7,7,-20,20
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ratio = (GLfloat)width / (GLfloat)height;
    if (width < height)
        glOrtho(-coordinatesize, coordinatesize, -coordinatesize / ratio, coordinatesize / ratio, -coordinatesize-13, coordinatesize + 13);
    else
        glOrtho(-coordinatesize * ratio, coordinatesize * ratio, -coordinatesize, coordinatesize, -coordinatesize-13, coordinatesize + 13);
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}

void idle() {
    angle += rotationSpeed;
    if (angle > 360.0f) {
        angle -= 360.0f;
    }
    else if (angle < 0.0f) {
        angle += 360.0f;
    }
    side_angle += sideSpeed;
    if (side_angle > 360.0f) {
        side_angle -= 360.0f;
    }
    else if (side_angle < 0.0f) {
        side_angle += 360.0f;
    }
    x_z_angle += x_z_Speed;
    if (x_z_angle > 360.0f) {
        x_z_angle -= 360.0f;
    }
    else if (x_z_angle <= 0.0f) {
        x_z_angle += 360.0f;
    }
    e_angle += e_Speed;
    if (e_angle > 360.0f) {
        e_angle -= 360.0f;
    }
    else if (e_angle <= 0.0f) {
        e_angle += 360.0f;
    }
    propeller_rotation += 2;
    planet_angle += 0.1;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'q' || key == 'Q')
        exit(0);
    else if (key == 'd' || key == 'D') {
        drone_x += 0.1f;
    }
    else if (key == 'a' || key == 'A') {
        drone_x -= 0.1f;
    }
    else if (key == 'w' || key == 'W') {
        drone_y += 0.1f;
    }
    else if (key == 's' || key == 'S') {
        drone_y -= 0.1f;
    }
    else if (key == 'e' || key == 'E') {
        drone_rotation += 10;
    }
    else if (key == 'z' || key == 'Z') {
        len_angle -= 1;
        if (len_angle <= 0) {
            len_angle = 0;
        }
    }
    else if (key == 'c' || key == 'C') {
        len_angle += 1;
        if (len_angle > 70) {
            len_angle = 70;
        }
    }
    else if (key == 'x' || key == 'X') {
        telescope_angle +=  10;
        if (telescope_angle > 170) {
            telescope_angle = 10;
        }
    }
    else if (key == 'v' || key == 'V') {
        cradle_step += 5;
    }
    else if (key == 'b' || key == 'B') {
        cradle_step -= 5;
    }
    else if (key == 'i' || key == 'I') {
        body_rotate += 5;
    }
    else if (key == 'l' || key == 'L') {
        body_move += 0.1;
        leg_swag += 2;
        if (leg_swag == 30) {
            arrive = GL_TRUE;
        }
        if (arrive) {
            leg_swag -= 4;
            if (leg_swag == -30) {
                arrive = GL_FALSE;
            }
        }
    }
    else if (key == 'j' || key == 'J') {
        body_move -= 0.1;
        leg_swag += 2;
        if (leg_swag == 30) {
            arrive = GL_TRUE;
        }
        if (arrive) {
            leg_swag -= 4;
            if (leg_swag == -30) {
                arrive = GL_FALSE;
            }
        }
    }
    else if (key == 'y')
        if (fltY0 > 35)
            fltY0 -= 5; //Camera down
}

// Respond to arrow keys
void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
        fltY0 += 0.2f;
    if (key == GLUT_KEY_DOWN)
        fltY0 -= 0.2f;
    if (key == GLUT_KEY_LEFT)
        fltX0 -= 0.2f;
    if (key == GLUT_KEY_RIGHT)
        fltX0 += 0.2f;
    // Refresh the window
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    int ModelMenu, WireMenu, MainMenu;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280,720);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Galaxy Observation");
    texture_init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(SpecialKeys);

    ModelMenu = glutCreateMenu(processMenu);
    glutAddMenuEntry("Icosahedron", 1);
    glutAddMenuEntry("Cube", 2);
    glutAddMenuEntry("Dodecahedron", 3);
    glutAddMenuEntry("Octahedron", 4);
    glutAddMenuEntry("Tetrahedron", 5);
    glutAddMenuEntry("Sphere", 6);

    WireMenu = glutCreateMenu(processMenu);
    glutAddMenuEntry("Wire", 7);
    glutAddMenuEntry("Solid", 8);

    MainMenu = glutCreateMenu(processMenu);
    glutAddSubMenu("Model", ModelMenu);
    glutAddSubMenu("WireOrSolid", WireMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}
