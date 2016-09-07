// 3D graphics program
#include "GLUT.H"
#include <math.h>
#include <time.h>

#define HEIGHT 600
#define WIDTH 600

const double PI = 4 * atan(1.0);

const int GSIZE = 150;

// Declarations
// Struct
struct Point {
	double x;
	double y;
	double z;
};


double ground[GSIZE][GSIZE];

double eyex = 0, eyez = 20, eyey = 5;
double dx = 0, dy = 0, dz = 0;
double speed = 0;
double dirx, dirz;
double sight = PI, angular_speed = 0;
double phasa = 0;
bool stop = false;
double offset;

double ax = 0, ay = 0, az = 0;
// controller position
int hook_control_top = HEIGHT - 60, hook_control_bottom = HEIGHT - 40, hook_control = HEIGHT - 50;
int rotate_control_left = 40, rotate_control_right = 60, rotate_control = 50;
int move_control_left = 40, move_control_right = 60, move_control = 50;

// calculated from the edges of the string 
int maxMoveHookUp = 0;
int minMoveHookDown = -10;
int stringLength = (minMoveHookDown + maxMoveHookUp) / 2;

// calculated from the edges of the crane 
int maxMovePlatform = 10;
int minMovePlarform = 2;

//hook position
Point hook;
double hookInitialStartX;
double hookInitialStartY;
double hookInitialStartZ;

// controller for hook
boolean hold = false;

void Smooth();

const int NUM_OF_BOXES = 9;
Point boxes[NUM_OF_BOXES];

bool boxIntersects(Point box, double x, double y, double z)
{
	if (box.x + 1 >= x && box.x - 1 <= x &&
		box.y + 1 >= y && box.y - 1 <= box.y &&
		box.z + 1 >= z && box.z - 1 <= box.z)
		return(true);
	else
		return(false);
}
int hookIntersects()
{
	for (int i = 0; i < NUM_OF_BOXES; i++)
	{
		if (boxIntersects(boxes[i], hook.x, hook.y, hook.z))
			return (i);
	}
	return (-1);
}

Point updatePoint(double x, double y, double z)
{
	Point newPoint;
	newPoint.x = x;
	newPoint.y = y;
	newPoint.z = z;
	return (newPoint);
}

// add to project properties->configuration ->linker->input
//->additional dependences-> opengl32.lib;glu32.lib;glut.lib;
void init()
{
	int i, j;
	srand(time(0)); // set random values

	for (i = 0; i<GSIZE; i++)
		for (j = 0; j<GSIZE; j++)
			ground[i][j] = 0;

	hook = updatePoint(0, (stringLength - 2), 0);//(minMovePlarform + maxMovePlatform) / 2);
	hookInitialStartX = hook.x;
	hookInitialStartY = hook.y;
	hookInitialStartZ = hook.z;
	// set background color
	glClearColor(0.61, 0.5, 0.9, 0);
	glEnable(GL_DEPTH_TEST);
}
void SetColor(double h)
{
	double g;

	g = fabs(h / 4);
	// grass - stones
	if (g<1)
		glColor3d(0.2 + 0.4*g, 0.6 - 0.4*g, 0);
	else // snow
		glColor3d(0.7*g, 0.7*g, 0.8*g);
}
void DrawCraneGround()
{
	int i, j;
	glColor3d(0, 1, 0);

	for (i = 0; i<GSIZE - 1; i++)
		for (j = 0; j<GSIZE - 1; j++)
		{
			glBegin(GL_POLYGON);// GL_LINE_LOOP);
			glVertex3d(j - GSIZE / 2, 0, i - GSIZE / 2);
			glVertex3d(j - GSIZE / 2, 0, i + 1 - GSIZE / 2);
			glVertex3d(j + 1 - GSIZE / 2, 0, i + 1 - GSIZE / 2);
			glVertex3d(j + 1 - GSIZE / 2, 0, i - GSIZE / 2);
			glEnd();
		}
	/*
	// draw Y axis
	glColor3d(1,0,0);
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(0,4,0);
	glEnd();
	*/
}
// Low-pass filter
void Smooth()
{
	double tmp[GSIZE][GSIZE];
	int i, j;

	for (i = 1; i<GSIZE - 1; i++)
		for (j = 1; j<GSIZE - 1; j++)
			tmp[i][j] = (ground[i - 1][j - 1] + ground[i - 1][j] + ground[i - 1][j + 1] +
				ground[i][j - 1] + ground[i][j] + ground[i][j + 1] +
				ground[i + 1][j - 1] + ground[i + 1][j] + ground[i + 1][j + 1]) / 9;

	for (i = 1; i<GSIZE - 1; i++)
		for (j = 1; j<GSIZE - 1; j++)
			ground[i][j] = tmp[i][j];
}
void DrawCube()
{
	// top
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0); // Red
	glVertex3d(-1, 1, 1);
	glColor3d(1, 1, 1); // White
	glVertex3d(1, 1, 1);
	glColor3d(0, 1, 0); // Green
	glVertex3d(1, 1, -1);
	glColor3d(1, 1, 0); // Yellow
	glVertex3d(-1, 1, -1);
	glEnd();
	// top
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 1); // Magenta
	glVertex3d(-1, -1, 1);
	glColor3d(0, 0, 1); // Blue
	glVertex3d(1, -1, 1);
	glColor3d(0, 1, 1); // Cyan
	glVertex3d(1, -1, -1);
	glColor3d(0, 0, 0); // Black
	glVertex3d(-1, -1, -1);
	glEnd();
	// front
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0); // Red
	glVertex3d(-1, 1, 1);
	glColor3d(1, 1, 1); // White
	glVertex3d(1, 1, 1);
	glColor3d(0, 0, 1); // Blue
	glVertex3d(1, -1, 1);
	glColor3d(1, 0, 1); // Magenta
	glVertex3d(-1, -1, 1);
	glEnd();
	// back
	glBegin(GL_POLYGON);
	glColor3d(0, 1, 0); // Green
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 1); // Cyan
	glVertex3d(1, -1, -1);
	glColor3d(0, 0, 0); // Black
	glVertex3d(-1, -1, -1);
	glColor3d(1, 1, 0); // Yellow
	glVertex3d(-1, 1, -1);
	glEnd();
	// left
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0); // Red
	glVertex3d(-1, 1, 1);
	glColor3d(1, 1, 0); // Yellow
	glVertex3d(-1, 1, -1);
	glColor3d(0, 0, 0); // Black
	glVertex3d(-1, -1, -1);
	glColor3d(1, 0, 1); // Magenta
	glVertex3d(-1, -1, 1);
	glEnd();
	// right
	glBegin(GL_POLYGON);
	glColor3d(1, 1, 1); // White
	glVertex3d(1, 1, 1);
	glColor3d(0, 1, 0); // Green
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 1); // Cyan
	glVertex3d(1, -1, -1);
	glColor3d(0, 0, 1); // Blue
	glVertex3d(1, -1, 1);
	glEnd();
}
void DrawCraneCube()
{
	double alpha;

	glBegin(GL_QUADS);

	// Define vertices in counter-clockwise (CCW) order with normal pointing out
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom face (y = -1.0f)
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face (x = -1.0f)
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face (x = 1.0f)
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube
}
void DrawCranePiece()
{
	// top
	glColor3d(0, 0, 0); // Black
	glBegin(GL_LINE_LOOP);
	glVertex3d(-1, 1, 1);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(1, 1, 1);
	glVertex3d(-1, 1, 1);
	glVertex3d(1, 1, -1);

	glEnd();
	// bottom
	glBegin(GL_LINE_LOOP);
	glVertex3d(-1, -1, 1);
	glVertex3d(1, -1, 1);
	glVertex3d(1, -1, -1);
	glVertex3d(-1, -1, -1);
	glVertex3d(1, -1, 1);
	glVertex3d(-1, -1, 1);
	glVertex3d(1, -1, -1);
	glEnd();
	// front
	glBegin(GL_LINE_LOOP);
	glVertex3d(-1, 1, 1);
	glVertex3d(1, 1, 1);
	glVertex3d(1, -1, 1);
	glVertex3d(-1, -1, 1);
	glVertex3d(1, 1, 1);
	glVertex3d(-1, 1, 1);
	glVertex3d(1, -1, 1);
	glEnd();
	// back
	glBegin(GL_LINE_LOOP);
	glVertex3d(1, 1, -1);
	glVertex3d(1, -1, -1);
	glVertex3d(-1, -1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(1, -1, -1);
	glVertex3d(1, 1, -1);
	glVertex3d(-1, -1, -1);
	glEnd();
	// left
	glBegin(GL_LINE_LOOP);
	glVertex3d(-1, 1, 1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, -1, -1);
	glVertex3d(-1, -1, 1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, 1, 1);
	glVertex3d(-1, -1, -1);
	glEnd();
	// right
	glBegin(GL_LINE_LOOP);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(1, -1, -1);
	glVertex3d(1, -1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(1, 1, 1);
	glVertex3d(1, -1, -1);
	glEnd();
}
void DrawCranePyramid()
{
	glColor3d(0, 0, 0); // Black
						// bottom
	glBegin(GL_LINE_LOOP);
	glVertex3d(-1, -1, 1);
	glVertex3d(1, -1, 1);
	glVertex3d(1, -1, -1);
	glVertex3d(-1, -1, -1);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(-1, -1, 1);
	glVertex3d(0, 1, 0);

	glVertex3d(1, -1, 1);
	glVertex3d(0, 1, 0);

	glVertex3d(1, -1, -1);
	glVertex3d(0, 1, 0);

	glVertex3d(-1, -1, -1);
	glVertex3d(0, 1, 0);

	glEnd();

}
void DrawBoxes()
{

	glPushMatrix();
	glColor3d(0.0, 1.0, 0.0);     // Green
	glTranslated(0, 1, 0);
	DrawCraneCube();
	glPopMatrix();
	boxes[0].x = 0;
	boxes[0].y = 1;
	boxes[0].z = 0;

	glPushMatrix();
	glColor3d(1.0, 0.5, 0.0);     // Orange
	glTranslated(0, 1, 2);
	DrawCraneCube();
	glPopMatrix();
	boxes[1].x = 0;
	boxes[1].y = 1;
	boxes[1].z = 2;

	glPushMatrix();
	glColor3d(1.0, 0.0, 0.0);     // Red
	glTranslated(2, 1, 0);
	DrawCraneCube();
	glPopMatrix();
	boxes[2].x = 2;
	boxes[2].y = 1;
	boxes[2].z = 0;

	glPushMatrix();
	glColor3d(1.0, 1.0, 0.0);     // Yellow
	glTranslated(0, 1, 4);
	DrawCraneCube();
	glPopMatrix();
	boxes[3].x = 0;
	boxes[3].y = 1;
	boxes[3].z = 4;


	glPushMatrix();
	glColor3d(0.0, 0.0, 1.0);     // Blue
	glTranslated(0, 1, 6);
	DrawCraneCube();
	glPopMatrix();

	boxes[4].x = 0;
	boxes[4].y = 1;
	boxes[4].z = 6;

	glPushMatrix();
	glColor3d(1.0, 0.0, 1.0);     // Magenta
	glTranslated(0, 1, 8);
	DrawCraneCube();
	glPopMatrix();

	boxes[5].x = 0;
	boxes[5].y = 1;
	boxes[5].z = 8;


	glPushMatrix();
	glColor3d(1, 1, 1);				// White
	glTranslated(0, 3, 0);
	DrawCraneCube();
	glPopMatrix();
	boxes[6].x = 0;
	boxes[6].y = 3;
	boxes[6].z = 0;

	glPushMatrix();
	glColor3d(0, 1, 1);				// Cyan
	glTranslated(0, 3, 2);
	DrawCraneCube();
	glPopMatrix();
	boxes[7].x = 0;
	boxes[7].y = 3;
	boxes[7].z = 2;

	glPushMatrix();
	glColor3d(0, 0, 0);				// Black
	glTranslated(0, 3, 4);
	DrawCraneCube();
	glPopMatrix();
	boxes[8].x = 0;
	boxes[8].y = 3;
	boxes[8].z = 4;
}
void DrawCraneLadder()
{
	glPushMatrix();
	glTranslated(4, 1, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePiece();
	glTranslated(0, 2, 0);
	DrawCranePyramid();
	glPopMatrix();
}
void DrawCraneHookPlatform()
{
	glColor3d(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3d(-1, 0, 1);
	glVertex3d(1, 0, 1);
	glVertex3d(1, 0, -1);
	glVertex3d(-1, 0, -1);
	glEnd();

}
void DrawHook()
{

	stringLength = (minMoveHookDown + maxMoveHookUp) / 2 + (hook_control - HEIGHT + 50)*(minMoveHookDown + maxMoveHookUp) / 80;
	glLineWidth(2);
	glColor3d(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	glVertex3d(0, 0, 0);
	glVertex3d(0, stringLength, 0);
	glVertex3d(0, stringLength - 0.25, 0);
	glVertex3d(0, stringLength - 0.5, -0.25);
	glVertex3d(0, stringLength - 1, 0);
	glVertex3d(0, stringLength - 0.5, 0.25);
	glEnd();
	glLineWidth(1);
}
void DrawCrane()
{
	int platformPosition = ((50 - move_control)*(minMovePlarform - maxMovePlatform) / 80 + (minMovePlarform + maxMovePlatform) / 2);
	//draw base
	glPushMatrix();
	glTranslated(4, 1, 0);
	glColor3d(0.5, 0.5, 0.5);
	DrawCraneCube();
	glPopMatrix();

	// TODO

	//hookInitialStartX = platformPosition;
	hookInitialStartZ = platformPosition;

	// Draw Crane Hook Platform
	glPushMatrix();
	// 80 is the range of the controll
	glTranslated(4, 11, platformPosition);
	DrawHook();
	DrawCraneHookPlatform();
	glPopMatrix();



	// Vertical Ladder
	DrawCraneLadder();

	// Horizontal Ladder
	glPushMatrix();
	glTranslated(0, 12, -4);
	glRotated(90, 1, 0, 0);
	DrawCraneLadder();
	glPopMatrix();

}
void ShowAll()
{
	//double radious = 
	double rotate = ((rotate_control - 50) * 360 / 80);
	double c = sqrt(hookInitialStartX*hookInitialStartX + hookInitialStartZ*hookInitialStartZ);
	int delta = hookInitialStartY - ((stringLength - 2));

	//update rotate
	hook = updatePoint(c*sin(rotate*PI / 180), hookInitialStartY - delta, c*cos(rotate*PI / 180));

	glEnable(GL_DEPTH_TEST);
	// start of the transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//DrawGround();
	DrawCraneGround();
	DrawBoxes();

	glPushMatrix();
	glTranslated(4, 1, 0);
	glRotated(rotate, 0, 1, 0);
	glTranslated(-4, -1, 0);
	DrawCrane();
	glPopMatrix();


	// pick box when press hold
	if (hold)
		if (hookIntersects() != -1)
			boxes[hookIntersects()] = updatePoint(hook.x, hook.y, hook.z);

	// draw red point
	glColor3d(1, 0, 0);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3d(hook.x + 4, hook.y + 12, hook.z);
	glEnd();

}
void controlCraneRotate()
{
	glViewport(100, 0, 100, 100);
	glMatrixMode(GL_PROJECTION); // setups the projection matrix
	glLoadIdentity();							// so that now it will be 2D
	glOrtho(-1, 1, -1, 1, -1, 1);


	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.9, 0.9, 0.4, 0.6);
	// frame
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glColor4d(1, 1, 1, 0.6);
	// slide
	glBegin(GL_POLYGON);
	glVertex2d(-0.9, 0.1);
	glVertex2d(0.9, 0.1);
	glVertex2d(0.9, -0.1);
	glVertex2d(-0.9, -0.1);
	glEnd();
	// control
	glColor4d(0, 0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex2d((rotate_control) / 50.0 - 1 - 0.1, -0.2);
	glVertex2d((rotate_control) / 50.0 - 1 + 0.1, -0.2);
	glVertex2d((rotate_control) / 50.0 - 1 + 0.1, 0.2);
	glVertex2d((rotate_control) / 50.0 - 1 - 0.1, 0.2);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW); // stop previous transformations
	glLoadIdentity();

}
void controlCraneLift()
{
	glViewport(200, 0, 100, 100);
	glMatrixMode(GL_PROJECTION); // setups the projection matrix
	glLoadIdentity();							// so that now it will be 2D
	glOrtho(-1, 1, -1, 1, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.9, 0.9, 0.4, 0.6);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glColor4d(1, 1, 1, 0.6);
	glBegin(GL_POLYGON);
	glVertex2d(-0.1, -0.9);
	glVertex2d(-0.1, 0.9);
	glVertex2d(0.1, 0.9);
	glVertex2d(0.1, -0.9);
	glEnd();
	// control
	glColor4d(0, 0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex2d(-0.2, (HEIGHT - hook_control) / 50.0 - 1 - 0.1);
	glVertex2d(-0.2, (HEIGHT - hook_control) / 50.0 - 1 + 0.1);
	glVertex2d(0.2, (HEIGHT - hook_control) / 50.0 - 1 + 0.1);
	glVertex2d(0.2, (HEIGHT - hook_control) / 50.0 - 1 - 0.1);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW); // stop previous transformations
	glLoadIdentity();

}
void controlCraneMove()
{
	glViewport(300, 0, 100, 100);
	glMatrixMode(GL_PROJECTION); // setups the projection matrix
	glLoadIdentity();							// so that now it will be 2D
	glOrtho(-1, 1, -1, 1, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.9, 0.9, 0.4, 0.6);
	// frame
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glColor4d(1, 1, 1, 0.6);
	// slide
	glBegin(GL_POLYGON);
	glVertex2d(-0.9, 0.1);
	glVertex2d(0.9, 0.1);
	glVertex2d(0.9, -0.1);
	glVertex2d(-0.9, -0.1);
	glEnd();
	// control
	glColor4d(0, 0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex2d((move_control) / 50.0 - 1 - 0.1, -0.2);
	glVertex2d((move_control) / 50.0 - 1 + 0.1, -0.2);
	glVertex2d((move_control) / 50.0 - 1 + 0.1, 0.2);
	glVertex2d((move_control) / 50.0 - 1 - 0.1, 0.2);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW); // stop previous transformations
	glLoadIdentity();

}
// regular view
void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 0.7, 300);
	gluLookAt(eyex, eyey, eyez,
		eyex + dirx, eyey - 0.5, eyez + dirz,
		0, 1, 0);

	ShowAll();
	// control 1
	controlCraneRotate();
	// control 2
	controlCraneLift();
	// control 3
	controlCraneMove();

	glutSwapBuffers();
}
void idle()
{
	double pitch = (hook_control - 510 - 40) * PI / 180;
	offset += 0.01;
	//	if(!stop)
	//		UpdateTerrain2();



	sight += angular_speed;
	dirx = sin(sight);
	dirz = cos(sight);

	eyex += dirx*speed;
	eyey += dy;
	eyez += dirz*speed;



	glutPostRedisplay(); //-> display
}
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		stop = !stop;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		Smooth();
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		angular_speed = 0;
		speed = 0;
		dy = 0;
		dx = 0;
		break;
	case 'h':
		hold = !hold;
		break;
	}
}
void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angular_speed += 0.005;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.005;
		break;
	case GLUT_KEY_UP:
		speed += 0.005;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.005;
		break;
	case GLUT_KEY_PAGE_UP:
		dy += 0.005;
		break;
	case GLUT_KEY_PAGE_DOWN:
		dy -= 0.005;
		break;
	}

}
void menu(int option)
{
	switch (option)
	{
		break;
	case 2:
		//		glutDisplayFunc(displayCockpit);
		break;
	case 3:

		break;
	case 4:

		break;
	default:
		break;
	}
}
void mouse_motion(int x, int y)
{
	if (x>40 + 200 && x<60 + 200 && y>hook_control_top && y<hook_control_bottom && y < HEIGHT - 10 && y > HEIGHT - 90)
	{
		hook_control = y;
		hook_control_top = hook_control - 10;
		hook_control_bottom = hook_control + 10;
	}
	if (y>HEIGHT - 60 && y < HEIGHT - 40 && x >rotate_control_left + 100 && x <rotate_control_right + 100 && x > 100 + 10 && x < 200 - 10)
	{
		rotate_control = x - 100;
		rotate_control_left = rotate_control - 10;
		rotate_control_right = rotate_control + 10;
	}
	if (y>HEIGHT - 60 && y < HEIGHT - 40 && x >move_control_left + 300 && x <move_control_right + 300 && x > 300 + 10 && x < 400 - 10)
	{
		move_control = x - 300;
		move_control_left = move_control - 10;
		move_control_right = move_control + 10;
	}
}
void main(int argc, char* argv[])
{
	// windowing
	glutInit(&argc, argv);
	// GLUT_DOUBLE stands for double buffer
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("3D");
	// set refresh function
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glutMotionFunc(mouse_motion);


	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Top View", 1);
	glutAddMenuEntry("Cockpit View", 2);
	glutAddMenuEntry("Regular View", 3);
	glutAddMenuEntry("Combined View", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop();
}
