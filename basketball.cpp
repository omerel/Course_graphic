// first graphics program
#include "GLUT.H"
#include <math.h>
#include <time.h>

#define HEIGHT 600
#define WIDTH 600

const double PI = 4*atan(1.0);

#define NUM_STARS 200

double stars[NUM_STARS];

double xball=9, yball=0, xo=0, yo=0, vx, vy, a=-0.05, t=0;
bool started = false;
double angle=0;

// add to project properties->configuration ->linker->input
//->additional dependences-> opengl32.lib;glu32.lib;glut.lib;
void init()
{
	int i;
	srand(time(0));
	
	for(i=0;i<NUM_STARS;i++)
		stars[i] = (rand()%1000)/1000.0;
	// set background color
	glClearColor(0.61,0.5,0.9,0);

	
	glOrtho(-1,1,-1,1,-1,1);
}

void DrawWheel()
{
	double x,y,alpha, offset = PI/50;
	glColor3d(1,1,1);

	// circle
	glBegin(GL_LINE_LOOP);
	for(alpha = 0; alpha <=2*PI; alpha+=offset)
	{
		x = cos(alpha);
		y = sin(alpha);
		glVertex2d(x,y);
	}
	glEnd();
	// lines
//	teta = PI/20;
	for(alpha = 0; alpha <=2*PI; alpha+=4*offset)
	{
	glBegin(GL_TRIANGLES);
		x = cos(alpha);
		y = sin(alpha);
		glColor3d(1-fabs(x),fabs(y),1-(fabs(x)+fabs(y))/2);
		glVertex2d(x,y);
		glVertex2d(0,0);
		x = cos(alpha+offset);
		y = sin(alpha+offset);
		glVertex2d(x,y);
	glEnd();
	}

}

void DrawBackground()
{
	double x,y;
	int i;
	// set gradient background
	glBegin(GL_POLYGON);
		glColor3d(0,0,0.5); // dark blue
		glVertex2d(-1,1);
		glVertex2d(1,1);
		glColor3d(0,1,1); // cyan
		glVertex2d(1,-1);
		glVertex2d(-1,-1);
	glEnd();

	// Draw graph
	glColor3d(0,0,0);
	glBegin(GL_LINE_STRIP);
		glVertex2d(-1,-0.5);
		glVertex2d(1,-0.5);
	glEnd();
	
	// random stars
	glColor3d(1,1,0);
	glBegin(GL_POINTS);
	for(i=0;i<NUM_STARS;i++)
		glVertex2d((2*i-NUM_STARS)/((double)NUM_STARS),stars[i]);
	glEnd();

}

void DrawBall()
{
	double alpha, beta, gama, x, y, teta=PI/20;
	glColor3d(1,0.6,0.2); // orange
	glBegin(GL_POLYGON);
	for (alpha=0; alpha<=2*PI; alpha+=teta)
	{
		x=cos(alpha);
		y=sin(alpha);
		glVertex2d(x,y);
	}
	glEnd();

	// Draw lines on ball
	// Cross
	glColor3d(0,0,0);
	glBegin(GL_LINES);
	glVertex2d(0,-1);
	glVertex2d(0,1);
	glVertex2d(-1,0);
	glVertex2d(1,0);
	glEnd();
	
	// Curved lines
	glPushMatrix();
	glTranslated(-1.4,0,0);
	glBegin(GL_LINE_STRIP);
	for (alpha=-PI/4; alpha<=PI/4; alpha+=teta)
	{
		x=cos(alpha);
		y=sin(alpha);
		glVertex2d(x,y);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
		glScaled(-1,1,1); // As above but flipped
		glTranslated(-1.4,0,0);
		glBegin(GL_LINE_STRIP);
		for (alpha=-PI/4; alpha<=PI/4; alpha+=teta)
		{
			x=cos(alpha);
			y=sin(alpha);
			glVertex2d(x,y);
		}
		glEnd();
	glPopMatrix();
}

void DrawBasket()
{
	double dx=0.05,y=0,dy=0.03;
	double a1,a2,b1,b2,x1=-0.9,x2=-0.65,y1=0.5,y2=0.2,x3=-0.6,y3=0.5,xt,yt, mid;

	// Draw Rim
	glColor3d(1,1,1);
	glBegin(GL_LINE_STRIP);
		glVertex2d(-1,y3);
		glVertex2d(x3,y3);
		glVertex2d(x2,y2);
		glVertex2d(x1+0.05,y2);
		glVertex2d(x1,y1);
	glEnd();

	// Draw Net

	// Calculate line values
	a1 = (y2 - y1) / (x2 - x1);
	a2 = (y3 - y2) / (x3 - x2);
	b2 = y2 - a2 * x2;

	// Right side
	glPushMatrix();
		for (x1=-0.9;x1<=-0.6;x1+=dx, y++)
		{
			b1 = y1 - a1 * x1;
			xt = (b2 - b1) / (a1 - a2);
			yt = a1 * xt + b1;
			glBegin(GL_LINES);
				glVertex2d(x1,y1);
				glVertex2d(xt,yt);
			glEnd();
		}
	glPopMatrix();

	// Left side
	x1=-0.9;
	mid = x1 + fabs(x1 - x3) / 2;
	glPushMatrix();
		glTranslated(mid,0,0);	// 3rd
		glScaled(-1,1,1);			// 2nd flip
		glTranslated(-1*mid,0,0);		// 1st move middle of basket to y-axis
		for (x1=-0.9;x1<=-0.6;x1+=dx, y++)
		{
			b1 = y1 - a1 * x1;
			xt = (b2 - b1) / (a1 - a2);
			yt = a1 * xt + b1;
			glBegin(GL_LINES);
				glVertex2d(x1,y1);
				glVertex2d(xt,yt);
			glEnd();
		}
	glPopMatrix();

}
// refresh
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// start of the transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	DrawBackground();
	glPushMatrix();
		if (started) // motion
			glTranslated(xball, yball, 0);
		glRotated(angle, 0, 0 ,1);
		glScaled(0.1,0.1,1);
		DrawBall();
	glPopMatrix();
	DrawBasket();

	glutSwapBuffers();

}

void idle()
{
	if(started)
	{
		(vx > 0) ? angle -= 0.5 : angle += 0.5;
		t += 0.01;
		xball = xo + vx * t;
		yball = yo + vy * t + a * t * t;

		// Check for collision
		if (xball - 0.105 < -1 || xball + 0.105 > 1)
		{
			vx = -0.9*vx;
			xo = xball;
			yo = yball;
			vy = vy + 2 * a * t;
			t = 0;
		}

		if (yball - 0.105 < -0.5 || yball + 0.105 > 1)
		{
			xo = xball;
			yo = yball;
			vy = -0.9*(vy + 2 * a * t);
			t = 0;
		}
	}

	glutPostRedisplay(); //-> display
}

void mouse(int button, int state, int x, int y)
{
	double px,py;  // Point clicked
	if(button==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		started = true;
		px = (2*((double)x)/WIDTH)-1;
		py = (2*((double)(HEIGHT-y))/HEIGHT)-1;;
		vx = px - xo;
		vy = py - yo;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if(key==VK_SPACE)
	{
		started = false;
	}
}

void main( int argc, char* argv[])
{
	// windowing
	glutInit(&argc,argv);
	// GLUT_DOUBLE stands for double buffer
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutInitWindowPosition(100,100);

	glutCreateWindow("First example");
	// set refresh function
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	init();

	glutMainLoop();
}
