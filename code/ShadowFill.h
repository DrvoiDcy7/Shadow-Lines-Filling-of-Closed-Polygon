#pragma once

#define WINDOW_HEIGHT 1000
#define NULL 0

#include <iostream>
#include <math.h>
using namespace std;

typedef struct Edge
{
	float x;     //coordinates of crossing point with the current scanning line
	float dx;    //incremental of current to the next scanning line
	int dmax;    //distance from vertices to y=kx
	struct Edge *next;    //point to the next edge
}Edge;

typedef struct Point
{
	int x;
	int y;
	int d;
}Point;

double k;    //gradient of scanning lines

			 //bresenham algorithm
void Bre_Line(int x0, int y0, int x1, int y1)
{
	int x, y, dx, dy, e;
	int i, temp;
	int change = 0;
	int Step_X = 1, Step_Y = 1;
	dx = x1 - x0;
	dy = y1 - y0;
	glBegin(GL_POINTS);
	if (dx<0)
	{
		dx = -dx;
		Step_X = -1;
	}
	if (dy<0)
	{
		dy = -dy;
		Step_Y = -1;
	}
	if (dy>dx)
	{
		temp = dx;
		dx = dy;
		dy = temp;
		change = 1;
	}
	e = -dx;
	x = x0;
	y = y0;
	for (i = 0; i <= dx; i++)    //Specify vertices
	{
		glVertex2i(x, y);
		if (change)    y = y + Step_Y;
		else          x = x + Step_X;
		e = e + 2 * dy;
		if (e >= 0)
		{
			if (change)    x = x + Step_X;
			else          y = y + Step_Y;
			e = e - 2 * dx;
		}
	}
	glEnd();
}

//Calculate distance from each vertex to y=kx
void CalculateD(int cnt, Point *pts)
{
	float m = sqrt((double)(1 + k*k));
	for (int i = 0; i<cnt; i++)
	{
		pts[i].d = abs(pts[i].y - k*pts[i].x) / m;
	}
}

//inserting sort for x-ordinate of crossing point increasingly
void InsertEdge(Edge *list, Edge *edge)
{
	Edge *p, *q = list;
	p = q->next;
	while (p)
	{
		if (edge->x < p->x)
			p = NULL;
		else
		{
			q = p;
			p = p->next;
		}
	}
	edge->next = q->next;
	q->next = edge;
}

//return d value of the next different value of d
int dNext(int i, int cnt, Point*pts)
{
	int j;
	if (i + 1 > cnt - 1)
		j = 0;
	else
		j = i + 1;
	while (pts[i].d == pts[j].d)
	{
		if (j + 1 > cnt - 1)
			j = 0;
		else
			j++;
	}
	return(pts[j].d);
}

//generate an edge for ordered edge table
void MakeEdgeRec(Point lower, Point higher, int dComp, Edge *edge, Edge *edges[])
{
	float k1;
	int db = 1;
	k1 = (float)(higher.y - lower.y) / (float)(higher.x - lower.x);
	k1 = k1 - k;
	edge->dx = db*sqrt((double)(1 + k*k)) / k1;
	edge->x = lower.x;
	if (higher.d < dComp)
		edge->dmax = higher.d - 1;    
	else
		edge->dmax = higher.d;    
	InsertEdge(edges[lower.d], edge);
}

//Create ordered edge table
void BuildEdgeList(int cnt, Point *pts, Edge *edges[])
{
	Edge *edge;
	Point v1, v2;
	int i, dPrev = pts[cnt - 2].d;
	v1 = pts[cnt - 1];
	for (i = 0; i<cnt; i++)
	{
		v2 = pts[i];
		if (v1.d != v2.d)
		{
			edge = (Edge*)malloc(sizeof(Edge));
			if (v1.d < v2.d)
				MakeEdgeRec(v1, v2, dNext(i, cnt, pts), edge, edges);    //next vertex
			else
				MakeEdgeRec(v2, v1, dPrev, edge, edges);
		}
		dPrev = v1.d;
		v1 = v2;
	}
}

//create active edge table
void BuildActiveList(int scan, Edge *active, Edge *edges[])
{
	Edge *p, *q;
	p = edges[scan]->next;
	while (p)
	{
		q = p->next;
		InsertEdge(active, p);
		p = q;
	}
}

//Fill with the current scanning lines
void FillScan(int scan, Edge *active)
{
	Edge *p1, *p2;
	p1 = active->next;
	while (p1)
	{
		p2 = p1->next;
		int x1 = p1->x;
		int x2 = p2->x;
		int y1 = k*x1 + scan*sqrt((float)(1 + k*k));
		int y2 = k*x2 + scan*sqrt((float)(1 + k*k));
		Bre_Line(x1, y1, x2, y2);
		p1 = p2->next;
	}
}

//delete edges not crossing
void DeleteAfter(Edge *q)
{
	Edge *p = q->next;
	q->next = p->next;
	free(p);
}

//refresh for the next scanning line
void UpdateActiveList(int scan, Edge *active)
{
	Edge *q = active, *p = active->next;
	while (p)
		if (scan >= p->dmax)
		{
			p = p->next;
			DeleteAfter(q);
		}
		else
		{
			p->x = p->x + p->dx;
			q = p;
			p = p->next;
		}
}

//Reorder the active edge table
void ResortActiveList(Edge *active)
{
	Edge *q;
	Edge *p = active->next;
	active->next = NULL;
	while (p)
	{
		q = p->next;
		InsertEdge(active, p);
		p = q;
	}
}

void ScanFill(int cnt, Point *pts)
{
	int db = 0;
	cout << "Please input the gradient of shadow lines(k<0)£º";
	cin >> k;
	int s;
	cout << "Please input the distance between shadow lines£º";
	cin >> s;
	CalculateD(cnt, pts);
	Edge *edges[WINDOW_HEIGHT], *active;
	int i, scan;
	for (i = 0; i<WINDOW_HEIGHT; i++)
	{
		edges[i] = (Edge*)malloc(sizeof(Edge));
		edges[i]->next = NULL;
	}
	BuildEdgeList(cnt, pts, edges);
	active = (Edge*)malloc(sizeof(Edge));
	active->next = NULL;
	for (scan = 0; scan<WINDOW_HEIGHT; scan++)
	{
		BuildActiveList(scan, active, edges);
		if (active->next)
		{
			if (db%s == 0)    //s indicates distance between two scanning lines
				FillScan(scan, active);
			UpdateActiveList(scan, active);
			ResortActiveList(active);
		}
		db++;
	}
}
