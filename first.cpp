// 3D graphics program
#include "GLUT.H"
#include <math.h>
#include <time.h>

#define HEIGHT 600
#define WIDTH 600

const double PI = 4 * atan(1.0);

const int GSIZE = 150;

double ground[GSIZE][GSIZE];

double eyex = 2, eyez = 35, eyey = 30;
double dx = 0, dy = 0, dz = 0;
double speed = 0;
double dirx, dirz;
double sight = PI, angular_speed = 0;
double phasa = 0;
bool stop = false;
double offset;


// Airplane
double airdirx, airdirz;
double airsight = 3 * PI / 2, airangular_speed = 0, airspeed = 0;
double ax = 0, ay = 0, az = 0;


// controller position
int hook_control_top = HEIGHT - 60, hook_control_bottom = HEIGHT - 40, hook_control = HEIGHT - 50;
int rotate_control_left =40, rotate_control_right =60, rotate_control = 50;
int move_control_left = 40, move_control_right = 60, move_control =50;

void UpdateTerrain2();
void Smooth();

// add to project properties->configuration ->linker->input
//->additional dependences-> opengl32.lib;glu32.lib;glut.lib;
void init()
{
	int i, j;

	srand(time(0)); // set random values

	for (i = 0; i<GSIZE; i++)
		for (j = 0; j<GSIZE; j++)
			ground[i][j] = 0;

	// generate random terrain
	for (i = 0; i<1700; i++)
		UpdateTerrain2();

	Smooth();
	Smooth();
	Smooth();

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

void DrawGround()
{
	int i, j;
	glColor3d(1, 1, 1);

	for (i = 0; i<GSIZE - 1; i++)
		for (j = 0; j<GSIZE - 1; j++)
		{
			glBegin(GL_POLYGON);// GL_LINE_LOOP);
			SetColor(ground[i][j]);
			glVertex3d(j - GSIZE / 2, ground[i][j], i - GSIZE / 2);
			SetColor(ground[i + 1][j]);
			glVertex3d(j - GSIZE / 2, ground[i + 1][j], i + 1 - GSIZE / 2);
			SetColor(ground[i + 1][j + 1]);
			glVertex3d(j + 1 - GSIZE / 2, ground[i + 1][j + 1], i + 1 - GSIZE / 2);
			SetColor(ground[i][j + 1]);
			glVertex3d(j + 1 - GSIZE / 2, ground[i][j + 1], i - GSIZE / 2);
			glEnd();
		}

	// draw Y axis
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 4, 0);
	glEnd();

	// water
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0, 0, 0.6, 0.7);
	glBegin(GL_POLYGON);
	glVertex3d(-GSIZE / 2, 0, -GSIZE / 2);
	glVertex3d(-GSIZE / 2, 0, GSIZE / 2);
	glVertex3d(GSIZE / 2, 0, GSIZE / 2);
	glVertex3d(GSIZE / 2, 0, -GSIZE / 2);
	glEnd();
	glDisable(GL_BLEND);

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
void UpdateTerrain1()
{
	int i, j;

	for (i = 0; i<GSIZE; i++)
		for (j = 0; j<GSIZE; j++)
			ground[i][j] += -0.05 + (rand() % 101) / 1000.0; // random numbers: [-0.05, 0.05] 

}
void UpdateTerrain2()
{
	int i, j;
	int x1, y1, x2, y2;
	double a, b;
	double delta = 0.1;

	if (rand() % 2 == 0)
		delta = -delta;

	x1 = rand() % GSIZE;
	y1 = rand() % GSIZE;
	x2 = rand() % GSIZE;
	y2 = rand() % GSIZE;

	if (x1 != x2)
	{
		a = ((double)y2 - y1) / (x2 - x1);
		b = y1 - a*x1;

		for (i = 0; i<GSIZE; i++)
			for (j = 0; j<GSIZE; j++)
			{
				if (i<a*j + b) ground[i][j] += delta;
				else ground[i][j] -= delta;
			}
	}

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
void DrawCylinder(int n)
{
	double alpha;
	double teta = 2 * PI / n;

	for (alpha = 0; alpha<2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha)), (1 + cos(alpha)) / 2, 0.5*cos(alpha));
		glVertex3d(sin(alpha), 1, cos(alpha));
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta));
		glColor3d(1 - fabs(sin(alpha)), 0.5*(1 + cos(alpha)) / 2, 0.8*cos(alpha));
		glVertex3d(sin(alpha + teta), -1, cos(alpha + teta));
		glVertex3d(sin(alpha), -1, cos(alpha));
		glEnd();
	}
}

void DrawCylinder1(int n, double topr, double bottomr)
{
	double alpha;
	double teta = 2 * PI / n;

	for (alpha = 0; alpha<2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha + PI / 2)), (1 + cos(alpha + PI / 2)) / 2, 0.5*cos(alpha + PI / 2));
		glVertex3d(topr*sin(alpha), 1, topr*cos(alpha));
		glVertex3d(topr*sin(alpha + teta), 1, topr*cos(alpha + teta));
		//				glColor3d(1-fabs(sin(alpha)),0.8*(1+cos(alpha))/2,0.6*cos(alpha));
		glVertex3d(bottomr*sin(alpha + teta), 0, bottomr*cos(alpha + teta));
		glVertex3d(bottomr*sin(alpha), 0, bottomr*cos(alpha));
		glEnd();
	}
}

void DrawSphere(int n, int l)
{
	double beta;
	double delta = PI / l;
	int i;

	for (beta = -PI / 2, i = 0; beta<PI / 2; beta += delta, i++)
	{
		glPushMatrix();
		//		glRotated(4*i*offset,0,1,0);
		glTranslated(0, sin(beta), 0);
		glScaled(1, (sin(beta + delta) - sin(beta)), 1);
		DrawCylinder1(n, cos(beta + delta), cos(beta));
		glPopMatrix();
	}
}

// sphere from start layer to stop layer
void DrawSphere1(int n, int l, int start, int stop)
{
	double beta;
	double delta = PI / l;
	int i;

	for (beta = -PI / 2 + start*delta, i = 0; beta<-PI / 2 + stop*delta; beta += delta, i++)
	{
		glPushMatrix();
		//	glRotated(20*i*offset,0,1,0);
		glTranslated(0, sin(beta), 0);
		glScaled(1, (sin(beta + delta) - sin(beta)), 1);
		DrawCylinder1(n, cos(beta + delta), cos(beta));
		glPopMatrix();
	}
}

void DrawAirplain()
{
	glPushMatrix();
	glTranslated(0, 150, 0);
	glRotated(90, 0, 0, 1);
	glPushMatrix();
	glScaled(3, 20, 3);
	DrawSphere1(30, 10, 1, 8);// 10 slices, start from slice 2  to slice 8
	glPopMatrix();
	glPushMatrix();

	glScaled(3, 10, 3);
	// we raise the conus by sin(-PI/2+8*PI/10) and multiply by 2
	// because the sphere was scaled by 20 while the conus was
	// scaled by 10 only
	// sin(-PI/2+8*PI/10) is the height of the end of cutted sphere
	glTranslated(0, 2 * sin(-PI / 2 + 8 * PI / 10), 0);
	DrawCylinder1(30, 0, cos(-PI / 2 + 8 * PI / 10));
	glPopMatrix();
	// cockpit
	glPushMatrix();
	glTranslated(0.8, 7.5, 0);
	glScaled(2.5, 10, 2.5);
	glRotated(180, 0, 1, 0);
	DrawSphere(16, 10);// 10 slices, start from slice 2  to slice 8
	glPopMatrix();
	// wings
	glPushMatrix();
	glTranslated(0, -7, 0);
	glScaled(0, 25, 12);
	DrawCylinder1(40, 0, 1);
	glPopMatrix();
	// rear wings
	glPushMatrix();
	glTranslated(0, -17, 0);
	glScaled(0, 10, 10);
	DrawCylinder1(40, 0, 1);
	glPopMatrix();
	// tail
	glBegin(GL_POLYGON);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glColor3d(0.4, 0, 0);
	glVertex3d(6, -17, 0);
	glVertex3d(6, -20, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, -18, 0);
	glEnd();
	glPopMatrix();

}

void DrawBoxes()
{
	glPushMatrix();
	glColor3d(0.0, 1.0, 0.0);     // Green
	glTranslated(0, 1, 0);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(1.0, 0.5, 0.0);     // Orange
	glTranslated(0, 1, 2);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(1.0, 0.0, 0.0);     // Red
	glTranslated(2, 1, 0);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(1.0, 1.0, 0.0);     // Yellow
	glTranslated(0, 1, 4);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(0.0, 0.0, 1.0);     // Blue
	glTranslated(0, 1, 6);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(1.0, 0.0, 1.0);     // Magenta
	glTranslated(0, 1, 8);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(1, 1, 1);				// White
	glTranslated(0, 3, 0);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(0, 1, 1);				// Cyan
	glTranslated(0, 3, 2);
	DrawCraneCube();
	glPopMatrix();
	glPushMatrix();
	glColor3d(0, 0, 0);				// Black
	glTranslated(0, 3, 4);
	DrawCraneCube();
	glPopMatrix();
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

void DrawCrane()
{
	// Base
	glColor3d(0.1, 0.1, 0.1);
	glBegin(GL_POLYGON);
	glVertex3d(-1, 0, 1);
	glVertex3d(1, 0, 1);
	glVertex3d(1, 0, -1);
	glVertex3d(-1, 0, -1);
	glEnd();

	// Vertical Ladder
	DrawCraneLadder();

	// Horizontal Ladder
	glPushMatrix();
	glTranslated(0, 12, -4);
	glRotated(90, 1, 0, 0);
	DrawCraneLadder();
	glPopMatrix();
	glColor3d(0.5, 0.5, 0.5);
	DrawCraneCube();
}

void ShowAll()
{
	//double pitch = hook_control - 510 - 40; // Range of hook_control is [-40, 40]
	double rotate = rotate_control;
	glEnable(GL_DEPTH_TEST);
	// start of the transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//DrawGround();
	DrawCraneGround();
	DrawBoxes();

	glPushMatrix();
		glTranslated(4, 1, 0);
		glRotated((rotate_control)*180/PI, 0, 1, 0);
		glTranslated(-4, -1, 0);
		DrawCrane();
	glPopMatrix();
	/*
	glPushMatrix();

	// Move
	glTranslated(ax, ay, az);

	// yaw
	glRotated((airsight) * 180 / PI + 90, 0, 1, 0);
	glScaled(0.2, 0.15, 0.2);

	glTranslated(0, 150, 0);
	// Pitch
	glRotated(pitch, 0, 0, 1);
	glTranslated(0, -150, 0);

	//DrawAirplain();
	glPopMatrix();
	*/
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
	glVertex2d((rotate_control) / 50.0 - 1 - 0.1 , -0.2);
	glVertex2d((rotate_control) / 50.0 - 1 + 0.1 , -0.2);
	glVertex2d((rotate_control) / 50.0 - 1 + 0.1 , 0.2);
	glVertex2d((rotate_control) / 50.0 - 1 - 0.1 , 0.2);
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

void displayTop()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 0.7, 300);
	gluLookAt(eyex, 50, eyez - 35,
		eyex + 0.001*dirx, 30, eyez - 35 + 0.001*dirz,
		0, 1, 0);

	ShowAll();

	glutSwapBuffers();
}

void displayCombined()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// regular view
	glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 0.7, 300);
	gluLookAt(eyex, eyey, eyez,
		eyex + dirx, eyey - 0.5, eyez + dirz,
		0, 1, 0);

	ShowAll();

	// top view
	glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 0.7, 300);
	gluLookAt(eyex, 50, eyez - 35,
		eyex + 0.001*dirx, 30, eyez - 35 + 0.001*dirz,
		0, 1, 0);

	ShowAll();

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


	airsight += airangular_speed;
	airdirx = sin(airsight);
	pitch = sin(pitch);
	airdirz = cos(airsight);

	ax += airdirx*airspeed;
	ay += sin(pitch) * airspeed;
	az += airdirz*airspeed;


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
	case 'w':
		airspeed += 0.0001;
		break;
	case 's':
		airspeed -= 0.0001;
		break;
	case 'a':
		airangular_speed += 0.001;
		break;
	case 'd':
		airangular_speed -= 0.001;
		break;

	case ' ':
		angular_speed = 0;
		speed = 0;
		dy = 0;
		dx = 0;
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
	case 1: // 
		glutDisplayFunc(displayTop);
		break;
	case 2:
		//		glutDisplayFunc(displayCockpit);
		break;
	case 3:
		glutDisplayFunc(display);
		break;
	case 4:
		glutDisplayFunc(displayCombined);
		break;
	default:
		break;
	}
}

void mouse_motion(int x, int y)
{
	if (x>40+200 && x<60+200 && y>hook_control_top && y<hook_control_bottom && y < HEIGHT - 10 && y > HEIGHT - 90)
	{
		hook_control = y;
		hook_control_top = hook_control - 10;
		hook_control_bottom = hook_control + 10;
	}
	if (y>HEIGHT-60 && y < HEIGHT -40 && x >rotate_control_left +100 && x <rotate_control_right+100 && x > 100+10 && x < 200-10)
	{
		rotate_control = x-100;
		rotate_control_left = rotate_control - 10;
		rotate_control_right = rotate_control + 10;
	}
	if (y>HEIGHT - 60 && y < HEIGHT - 40 && x >move_control_left + 300 && x <move_control_right + 300 && x > 300+10 && x < 400-10)
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
