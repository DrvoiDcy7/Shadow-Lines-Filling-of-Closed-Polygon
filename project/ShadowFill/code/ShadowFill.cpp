// ShadowFill.cpp : entrance
//

#include "stdafx.h"
#include <gl/glut.h>
#include "ShadowFill.h"
#include <iostream>
using namespace std;

int n;
Point *p;
bool select = false;

void myInit()
{
	glClearColor(0.9, 0.8, 0.9, 0.0);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 360.0);
}


void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (select == true)
	{
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i<n; i++)
			glVertex2i(p[i].x, p[i].y);
		glEnd();
		ScanFill(n, p);
	}
	glutSwapBuffers();
	glFlush();
}

void myMouse(int button, int state, int x, int y)    //Record vertices by mouse clicking
{
	static int number = 0;
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)    //Specify a vertex by clicking the left button
		{
			p[number].x = x;
			p[number].y = 360 - y;
			glBegin(GL_POINTS);
			glVertex2i(x, 360 - y);
			glEnd();
			glFlush();
			cout << p[number].x << " " << p[number].y << endl;
			number++;
			if (number == n)
			{
				number = 0;
				select = true;
				glutPostRedisplay();
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)    //Refresh by clicking the right button
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
		}
	}
}

int main(int argc, char *argv[])
{
	cout << "Please input the point number of the polygon" << endl;
	cin >> n;
	p = (Point*)malloc(n * sizeof(Point));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(600, 180);
	glutInitWindowSize(640, 360);
	glutCreateWindow("Shadow line filling of a closed polygon");
	glutMouseFunc(myMouse);
	glutDisplayFunc(myDisplay);
	myInit();
	glutMainLoop();
	return 0;
}

