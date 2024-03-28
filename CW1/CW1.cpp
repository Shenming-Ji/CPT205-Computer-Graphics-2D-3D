// File ID: cw1.cpp
// Title: 2024 Ceremony
// Author: Shenming
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <windows.h>
#define MAX_CHAR 128
#include <map>
#include <string>
#define PI 3.14159265
#include <math.h>

bool m_flag = true;
typedef struct { GLfloat x, y; } point;                                        // define a 2D point
point p0 = { 10, 560 };                                                        // set initial co-ordinate values of the point  
std::map < std::string, GLuint> fontDisplayLists;                              // change the drawString and selectFont to adapt to different font anb size
GLuint currentListID = 0;
static int isFirstCall = 1;
int n = 200;                                                                   // The number of division used to construct a circle
int time_interval = 16;                                                        // declare refresh interval in ms
GLfloat t = 0;                                                                 // rotation degree of sky wheel
GLfloat delta = 0;                                                             // degree of rotation             
GLfloat t_step = 1;                                                            // declare incremental step
GLfloat step = 0.5;                                                            // declare incremental step
GLfloat star_y = 450, starStep = 0;                                            // location and step of initial stars
GLfloat viewPortx = 0, viewPorty = 0, viewPWidth = 1280;                       // viewport's position and size
GLfloat x_step = 0, y_step = 0, w_step = 0;                                    // change the viewport to simulate the rising moon 
GLfloat m_start = -450, m_step = 0;											   // moon moving parameters

void display();
void selectFont(int size, int charset, const char* face);
void drawString(const char* str);
void CB_Building();
void IR_Building();
void AirPlane();
void Star_Part(GLfloat ox, GLfloat oy, GLfloat x, GLfloat y);
void Star(GLfloat ox, GLfloat oy, GLfloat x, GLfloat y);
void rotate_star(GLfloat ox, GLfloat oy, GLfloat* x, GLfloat* y, GLfloat delta);
void display(void);
void when_in_mainloop();
void OnTimer(int value);
void keyboard_input(unsigned char key, int x, int y);
void mouse_input(int button, int state, int x, int y);
void sky_wheel();
void Circle(int R, int n, GLfloat x, GLfloat y, int flag);

void when_in_mainloop()
{						 // idle callback function
	glViewport(viewPortx, viewPorty, viewPWidth, viewPWidth * 0.5625);
	glutPostRedisplay(); // force OpenGL to redraw the current window
}

void OnTimer(int value)
{
	p0.x += step;
	if (p0.x >= 280 && p0.x <= 500)
		p0.x = 500;
	else if (p0.x > 800)
		p0.x = 0;

	star_y -= starStep;
	if (star_y <= 0) {
		starStep = 0;
		star_y = 0;
	}

	t += t_step;
	if (t >= 360)
		t = 0;
	else if (t <= 0)
		t = 359;

	viewPortx -= x_step;
	viewPorty -= y_step;
	viewPWidth += w_step;
	m_start += m_step;
	if (viewPortx <= -680) {
		x_step = 0;
		w_step = 0;
		m_step = 0.8;
		y_step = m_step;
	}
	if (m_start >= 0) {
		m_step = 0;
		y_step = -2;
		x_step = 0;
		w_step = 0;
		m_flag = false;
	}
	if (viewPorty >= 0 && m_flag == false) {
		y_step = 0;
		x_step = -4;
		w_step = -8;
	}
	if (viewPortx == 0 && viewPWidth == 1280 && m_flag == false) {
		x_step = 0;
		w_step = 0;
	}
	glutTimerFunc(time_interval, OnTimer, 1);
}
void keyboard_input(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q')
		exit(0);
	else if (key == 's' || key == 'S') // stop movement
		step = 0;
	else if (key == 'r' || key == 'R') // reset step (resume movement)
		step = 0.5;
	else if (key == 'c' || key == 'C')// change direction of movement
		t_step = -t_step;
	else if (key == 'a' || key == 'A')// set step
		t_step = 3;
	else if (key == 'd' || key == 'D')// stop movement
		t_step = 0;
	else if (key == ' ')              // active stars
		starStep = 1.8;
	else if (key == 'l' || key == 'L' && m_start == -450) {
		x_step = 4;
		w_step = 8;
	}
}
void mouse_input(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && t_step >= 1)
		t_step -= 1; // decrement step
	else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON && t_step <= 3)
		t_step += 1; // increment step
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("2024 XJTLU");
	glutDisplayFunc(display);
	glutIdleFunc(when_in_mainloop);
	glutTimerFunc(time_interval, OnTimer, 1);
	glutKeyboardFunc(keyboard_input);
	glutMouseFunc(mouse_input);
	glutMainLoop();
}
// Display the graph
void display(void) {
	// initialization
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	gluOrtho2D(0, 800, 0, 600);
	// Set a XJTLU Gradient color background
	glBegin(GL_QUADS);
	// Blue
	glColor3f(0.5, 0.7, 1);
	glVertex2f(0, 0);
	glVertex2f(800, 0);
	glColor3f(0.7, 0.3, 1);
	glVertex2f(800, 300);
	glVertex2f(0, 300);
	// Purple
	glColor3f(0.7, 0.3, 1);
	glVertex2f(0, 300);
	glVertex2f(800, 300);
	glColor3f(0.5, 0, 0.7);
	glVertex2f(800, 600);
	glVertex2f(0, 600);
	glEnd();
	// Ceremony Gist
	glColor3f(1, 1, 1);
	selectFont(160, ANSI_CHARSET, "Edwardian Script ITC");
	glRasterPos2f(315, 530);
	drawString("2024");
	glColor3f(1, 1, 1);
	selectFont(60, ANSI_CHARSET, "Comic Sans MS");
	glRasterPos2f(110, 480);
	drawString("XI'AN JIAOTONG-LIVERPOOL UNIVERSITY");
	glColor3f(1, 1, 1);
	selectFont(60, ANSI_CHARSET, "Comic Sans MS");
	glRasterPos2f(240, 420);
	drawString("GRADUATION CEREMONY");
	// Motto
	glColor3f(0.7, 0.3, 1);
	selectFont(60, ANSI_CHARSET, "Comic Sans MS");
	glRasterPos2f(520, 20);
	drawString("Light and Wings");
	// moon 
	glColor3f(1, 0.9, 0);
	Circle(30, 200, 520, 380, 2);
	// Central Building
	CB_Building();
	// Internation Research
	IR_Building();
	//IR_Light();
	// Airplane 
	glEnable(GL_LINE_SMOOTH);
	AirPlane();
	glDisable(GL_LINE_SMOOTH);
	// Stars
	Star(30, star_y + 450, 30, star_y + 470);
	Star(130, star_y + 565, 130, star_y + 580);
	Star(230, star_y + 410, 230, star_y + 425);
	Star(370, star_y + 470, 370, star_y + 480);
	Star(470, star_y + 360, 470, star_y + 375);
	Star(770, star_y + 465, 770, star_y + 480);
	Star(590, star_y + 390, 590, star_y + 405);
	Star(700, star_y + 530, 700, star_y + 550);
	// Suchoow Sky Wheel
	sky_wheel();
	glFlush();
}
// Drawing text
void drawString(const char* str) {

	for (; *str != '\0'; ++str) {
		glCallList(currentListID + *str);
	}
}
// Select different fonts
void selectFont(int size, int charset, const char* face) {
	std::string fontFace(face);
	if (fontDisplayLists.find(fontFace) == fontDisplayLists.end()) {
		HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
		HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);

		GLuint newListID = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, newListID);

		fontDisplayLists[fontFace] = newListID; // Store the new list ID in the map.

		DeleteObject(hOldFont);
		currentListID = newListID; // Set the current display list ID.
	}
	else {
		// Font is in the map. Just update the current list ID.
		currentListID = fontDisplayLists[fontFace];
	}
}
// Central Building
void CB_Building() {
	// Central Building
	// Base
	glLineWidth(2.0);
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(790, 70);
	glVertex2f(790, 95);
	glVertex2f(450, 95);
	glVertex2f(395, 40);
	glVertex2f(395, 35);
	glVertex2f(450, 70);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(450, 70);
	glVertex2f(450, 95);
	glVertex2f(400, 0);
	glVertex2f(400, 38);
	glVertex2f(460, 0);
	glVertex2f(460, 70);
	glVertex2f(783, 0);
	glVertex2f(783, 70);
	glEnd();
	// Left hand side Part
	glBegin(GL_LINE_LOOP);
	glVertex2f(480, 95);
	glVertex2f(477, 120);
	glVertex2f(472, 125);
	glVertex2f(458, 260);
	glVertex2f(480, 284);
	glVertex2f(490, 270);
	glVertex2f(500, 280);
	glVertex2f(502, 303);
	glVertex2f(560, 370);
	glVertex2f(560, 190);
	glVertex2f(535, 160);
	glVertex2f(500, 130);
	glVertex2f(500, 110);
	glVertex2f(510, 118);
	glVertex2f(535, 150);
	glVertex2f(560, 165);
	glVertex2f(560, 95);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(483, 280);
	glVertex2f(502, 299);
	glEnd();
	// Right hand side Part
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(560, 370);
	glVertex2f(660, 335);
	glVertex2f(657, 190);
	glVertex2f(652, 185);
	glVertex2f(560, 190);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(560, 165);
	glVertex2f(645, 162);
	glVertex2f(645, 162);
	glVertex2f(765, 120);
	glVertex2f(765, 120);
	glVertex2f(764, 95);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(680, 328);
	glVertex2f(775, 298);
	glVertex2f(768, 160);
	glVertex2f(762, 154);
	glVertex2f(718, 162);
	glVertex2f(700, 180);
	glVertex2f(678, 220);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(660, 328);
	glVertex2f(680, 322);
	glVertex2f(760, 155);
	glVertex2f(759, 122);
	glEnd();
	// CB Decoration
	glBegin(GL_LINES);
	glVertex2f(463, 223);
	glVertex2f(560, 322);
	glVertex2f(467, 182);
	glVertex2f(560, 274);
	glVertex2f(471, 140);
	glVertex2f(560, 226);
	glVertex2f(479, 102);
	glVertex2f(500, 122);
	glVertex2f(510, 95);
	glVertex2f(560, 130);
	glVertex2f(560, 322);
	glVertex2f(659, 294);
	glVertex2f(560, 274);
	glVertex2f(658, 253);
	glVertex2f(560, 226);
	glVertex2f(657, 212);
	glVertex2f(560, 130);
	glVertex2f(765, 109);
	glVertex2f(680, 290);
	glVertex2f(774, 265);
	glVertex2f(679, 250);
	glVertex2f(772, 231);
	glVertex2f(685, 208);
	glVertex2f(770, 191);
	glEnd();
}
// Internation Research Center
void IR_Building() {
	//// IR
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(10, 0);
	glVertex2f(10, 340);
	glVertex2f(240, 340);
	glVertex2f(240, 0);
	glEnd();
	// top
	glBegin(GL_LINE_LOOP);
	glVertex2f(50, 340);
	glVertex2f(50, 350);
	glVertex2f(30, 350);
	glVertex2f(30, 390);
	glVertex2f(220, 390);
	glVertex2f(220, 350);
	glVertex2f(200, 350);
	glVertex2f(200, 340);
	glEnd();
	// out lines
	glBegin(GL_LINES);
	glVertex2f(30, 0);
	glVertex2f(30, 340);
	glVertex2f(220, 0);
	glVertex2f(220, 340);
	glVertex2f(50, 350);
	glVertex2f(200, 350);
	glEnd();
	// top
	glBegin(GL_LINE_LOOP);
	glVertex2f(40, 360);
	glVertex2f(40, 380);
	glVertex2f(210, 380);
	glVertex2f(210, 360);
	glEnd();
	// bold lines
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(20, 10);
	glVertex2f(20, 140);
	glVertex2f(20, 330);
	glVertex2f(20, 200);
	glVertex2f(230, 10);
	glVertex2f(230, 140);
	glVertex2f(230, 330);
	glVertex2f(230, 200);
	glEnd();
	// center rectangle
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(100, 100);
	glVertex2f(100, 180);
	glVertex2f(150, 180);
	glVertex2f(150, 100);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(105, 105);
	glVertex2f(105, 175);
	glVertex2f(145, 175);
	glVertex2f(145, 105);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(100, 240);
	glVertex2f(100, 320);
	glVertex2f(150, 320);
	glVertex2f(150, 240);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(105, 245);
	glVertex2f(105, 315);
	glVertex2f(145, 315);
	glVertex2f(145, 245);
	glEnd();
	// horizontal lines
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2f(30, 320);
	glVertex2f(220, 320);
	glVertex2f(30, 290);
	glVertex2f(100, 290);
	glVertex2f(150, 290);
	glVertex2f(220, 290);
	glVertex2f(30, 260);
	glVertex2f(100, 260);
	glVertex2f(150, 260);
	glVertex2f(220, 260);
	glVertex2f(30, 230);
	glVertex2f(220, 230);
	glVertex2f(30, 200);
	glVertex2f(220, 200);
	glVertex2f(30, 170);
	glVertex2f(100, 170);
	glVertex2f(150, 170);
	glVertex2f(220, 170);
	glVertex2f(30, 140);
	glVertex2f(100, 140);
	glVertex2f(150, 140);
	glVertex2f(220, 140);
	glVertex2f(30, 110);
	glVertex2f(100, 110);
	glVertex2f(150, 110);
	glVertex2f(220, 110);
	glVertex2f(30, 80);
	glVertex2f(220, 80);
	glVertex2f(30, 50);
	glVertex2f(220, 50);
	glVertex2f(30, 20);
	glVertex2f(220, 20);
	glEnd();
}
// mouse interaction airplane
void AirPlane() {
	glBegin(GL_LINE_LOOP);
	glVertex2f(p0.x, p0.y);
	glVertex2f(p0.x - 5, p0.y + 15);
	glVertex2f(p0.x, p0.y + 15);
	glVertex2f(p0.x + 3, p0.y + 5);
	glVertex2f(p0.x + 23, p0.y + 5);
	glVertex2f(p0.x + 11, p0.y + 36);
	glVertex2f(p0.x + 16, p0.y + 34);
	glVertex2f(p0.x + 21, p0.y + 24);
	glVertex2f(p0.x + 25, p0.y + 24);
	glVertex2f(p0.x + 25, p0.y + 21);
	glVertex2f(p0.x + 23, p0.y + 21);
	glVertex2f(p0.x + 27, p0.y + 15);
	glVertex2f(p0.x + 30, p0.y + 15);
	glVertex2f(p0.x + 30, p0.y + 13);
	glVertex2f(p0.x + 28, p0.y + 13);
	glVertex2f(p0.x + 35, p0.y + 5);
	glVertex2f(p0.x + 45, p0.y + 5);
	glVertex2f(p0.x + 50, p0.y);
	glVertex2f(p0.x + 45, p0.y - 5);
	glVertex2f(p0.x + 35, p0.y - 5);
	glVertex2f(p0.x + 28, p0.y - 13);
	glVertex2f(p0.x + 30, p0.y - 13);
	glVertex2f(p0.x + 30, p0.y - 15);
	glVertex2f(p0.x + 27, p0.y - 15);
	glVertex2f(p0.x + 23, p0.y - 21);
	glVertex2f(p0.x + 25, p0.y - 21);
	glVertex2f(p0.x + 25, p0.y - 24);
	glVertex2f(p0.x + 21, p0.y - 24);
	glVertex2f(p0.x + 16, p0.y - 34);
	glVertex2f(p0.x + 11, p0.y - 36);
	glVertex2f(p0.x + 23, p0.y - 5);
	glVertex2f(p0.x + 3, p0.y - 5);
	glVertex2f(p0.x, p0.y - 15);
	glVertex2f(p0.x - 5, p0.y - 15);
	glEnd();
}
// Creat a one fifth star, namely a polygon, based on the position of a central point and a top point 
void Star_Part(GLfloat ox, GLfloat oy, GLfloat x, GLfloat y) {
	GLfloat p1 = tan(PI / 10) / (tan(PI / 10) + tan(PI / 5));
	GLfloat p2 = tan(PI / 5);
	GLfloat x1 = p1 * (x - ox) + p1 * p2 * (y - oy) + ox;
	GLfloat y1 = p1 * (y - oy) + p1 * p2 * (ox - x) + oy;
	GLfloat x2 = p1 * (x - ox) - p1 * p2 * (y - oy) + ox;
	GLfloat y2 = p1 * (y - oy) - p1 * p2 * (ox - x) + oy;
	glColor3f(0.5, 0.7, 1);
	glBegin(GL_POLYGON);
	glVertex3f(ox, oy, 0.5);
	glVertex3f(x1, y1, 0.5);
	glVertex3f(x, y, 0.5);
	glVertex3f(x2, y2, 0.5);
	glEnd();
	glColor3f(1, 1, 1);
}
// Rotate delta to construct a complete star. Using pointers to keep iteration.
void rotate_star(GLfloat ox, GLfloat oy, GLfloat* x, GLfloat* y, GLfloat delta) {
	GLfloat rx = cos(delta) * (*x - ox) + sin(delta) * (*y - oy);
	GLfloat ry = cos(delta) * (*y - oy) + sin(delta) * (ox - *x);
	*x = rx + ox;
	*y = ry + oy;
}
// Loop to form a complete star
void Star(GLfloat ox, GLfloat oy, GLfloat x, GLfloat y) {
	GLfloat delta = 2 * PI / 5;
	for (int i = 0; i < 5; i++) {
		Star_Part(ox, oy, x, y);
		rotate_star(ox, oy, &x, &y, delta);
	}
}
// Rotatable sky wheel utilising translating and rotating operation
void sky_wheel() {
	// Base
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(365, 0);
	glVertex2f(365, 5);
	glVertex2f(265, 5);
	glVertex2f(265, 0);
	glEnd();
	// Skeleton
	glBegin(GL_LINES);
	glVertex2f(290, 5);
	glVertex2f(315, 80);
	glVertex2f(295, 5);
	glVertex2f(315, 80);
	glVertex2f(335, 5);
	glVertex2f(315, 80);
	glVertex2f(340, 5);
	glVertex2f(315, 80);
	glEnd();
	// Main circles
	Circle(25, 200, 315, 80, 0);
	Circle(27, 200, 315, 80, 0);
	Circle(49, 200, 315, 80, 0);
	Circle(52, 200, 315, 80, 0);
	// circle like seats
	// begin to rotate
	glPushMatrix();
	glTranslatef(315, 80, 0);
	glRotatef(t, 0, 0, 1);
	glColor3f(1, 1, 1);
	// 1
	Circle(3, 200, 50 * cos(0 * PI / 4), 50 * sin(0 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(0 * PI / 4), 50 * sin(0 * PI / 4));
	glEnd();
	// 2
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(1 * PI / 4), 50 * sin(1 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(1 * PI / 4), 50 * sin(1 * PI / 4));
	glEnd();
	// 3
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(2 * PI / 4), 50 * sin(2 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(2 * PI / 4), 50 * sin(2 * PI / 4));
	glEnd();
	// 4
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(3 * PI / 4), 50 * sin(3 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(3 * PI / 4), 50 * sin(3 * PI / 4));
	glEnd();
	// 5
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(4 * PI / 4), 50 * sin(4 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(4 * PI / 4), 50 * sin(4 * PI / 4));
	glEnd();
	// 6
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(5 * PI / 4), 50 * sin(5 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(5 * PI / 4), 50 * sin(5 * PI / 4));
	glEnd();
	// 7
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(6 * PI / 4), 50 * sin(6 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(6 * PI / 4), 50 * sin(6 * PI / 4));
	glEnd();
	// 8
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Circle(3, 200, 50 * cos(7 * PI / 4), 50 * sin(7 * PI / 4), 1);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(7 * PI / 4), 50 * sin(7 * PI / 4));
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}
// Creat circle like sightseeing capsules
void Circle(int R, int n, GLfloat x, GLfloat y, int flag) {
	// No need to fill in the color
	if (flag == 0) {
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 200; i++) {
			glVertex2f(R * cos(2 * PI / n * i) + x, R * sin(2 * PI / n * i) + y);
		}
		glEnd();
	}
	// Need to fill the color
	else if (flag == 1) {
		//glColor3f(1, 1, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 200; i++) {
			glVertex2f(R * cos(2 * PI / n * i) + x, R * sin(2 * PI / n * i) + y);
		}
		glEnd();
	}
	else {
		glBegin(GL_POLYGON);
		for (int i = 0; i < 200; i++) {
			glVertex2f(R * cos(2 * PI / n * i) + x, R * sin(2 * PI / n * i) + y + m_start);
		}
		glEnd();
	}
}