#include <iostream>
#include <vector>
#include "screen.h"
struct vec3d
{
	double x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	vector<triangle> tris;
};

struct mat4x4
{
	double m[4][4] = { 0 };
};

struct Point2d
{
	double x, y;
};

void Multiply_matrix(vec3d& i, vec3d& o, mat4x4& m) {
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0) {
		o.x /= w; o.y /= w; o.z /= w;
	}
}

vector<Point2d> DDA(Point2d p1, Point2d p2) {
	vector<Point2d> result;
	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = p2.x;
	float y2 = p2.y;
	int k;

	float dx = x2 - x1;
	float dy = y2 - y1;

	if (abs(dx) >= abs(dy))
		k = abs(dx);
	else
		k = abs(dy);
	if (k != 0) {
		float xinc = dx / k;
		float yinc = dy / k;

		float x = x1;
		float y = y1;
		Point2d p;
		p.x = x;
		p.y = y;
		result.push_back(p);

		for (int i = 1; i < k; i++)
		{
			x = x + xinc;
			y = y + yinc;
			Point2d p;
			p.x = x;
			p.y = y;
			result.push_back(p);
		}
	}
	
	return result;
}
vector<Point2d> Operate(mat4x4& matRotX, int width, int height) {
	vector<Point2d> result;
	mesh meshcoor;
	mat4x4 matProj;

	meshcoor.tris = {
		// SOUTH
		{ 0.0, 0.0, 0.0,    0.0, 1.0, 0.0,    1.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0,    1.0, 1.0, 0.0,    1.0, 0.0, 0.0 },

		// EAST                                                      
		{ 1.0, 0.0, 0.0,    1.0, 1.0, 0.0,    1.0, 1.0, 1.0 },
		{ 1.0, 0.0, 0.0,    1.0, 1.0, 1.0,    1.0, 0.0, 1.0 },

		// NORTH                                                     
		{ 1.0, 0.0, 1.0,    1.0, 1.0, 1.0,    0.0, 1.0, 1.0 },
		{ 1.0, 0.0, 1.0,    0.0, 1.0, 1.0,    0.0, 0.0, 1.0 },

		// WEST                                                      
		{ 0.0, 0.0, 1.0,    0.0, 1.0, 1.0,    0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 1.0,    0.0, 1.0, 0.0,    0.0, 0.0, 0.0 },

		// TOP                                                      
		{ 0.0, 1.0, 0.0,    0.0, 1.0, 1.0,    1.0, 1.0, 1.0 },
		{ 0.0, 1.0, 0.0,    1.0, 1.0, 1.0,    1.0, 1.0, 0.0 },

		// BOTTOM                                                    
		{ 1.0, 0.0, 1.0,    0.0, 0.0, 1.0,    0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 1.0,    0.0, 0.0, 0.0,    1.0, 0.0, 0.0 },
	};

	//Set up projection matrix
	double fNear = 0.1;
	double fFar = 1000.0;
	double fov = 90.0;
	double fAspectRatio = float(width) / float(height);
	//Fov to radiant
	double fFovRad = 1.0 / tan(fov * 0.5 / 180.0 * 3.14159);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0;
	matProj.m[3][3] = 0.0;

	//Process Triangle
	for (auto& tri : meshcoor.tris) {
		triangle triprojected, tritranslated, triRotatedZ;

		Multiply_matrix(tri.p[0], triRotatedZ.p[0], matRotX);
		Multiply_matrix(tri.p[1], triRotatedZ.p[1], matRotX);
		Multiply_matrix(tri.p[2], triRotatedZ.p[2], matRotX);


		//Translate triangle
		tritranslated = triRotatedZ;
		tritranslated.p[0].z = triRotatedZ.p[0].z + 3;
		tritranslated.p[1].z = triRotatedZ.p[1].z + 3;
		tritranslated.p[2].z = triRotatedZ.p[2].z + 3;

		//Calculate triangle
		Multiply_matrix(tritranslated.p[0], triprojected.p[0], matProj);
		Multiply_matrix(tritranslated.p[1], triprojected.p[1], matProj);
		Multiply_matrix(tritranslated.p[2], triprojected.p[2], matProj);

		//Scale
		triprojected.p[0].x += 1; triprojected.p[0].y += 1;
		triprojected.p[1].x += 1; triprojected.p[1].y += 1;
		triprojected.p[2].x += 1; triprojected.p[2].y += 1;

		triprojected.p[0].x *= 0.5 * float(height); triprojected.p[0].y *= 0.5 * float(height);
		triprojected.p[1].x *= 0.5 * float(height); triprojected.p[1].y *= 0.5 * float(height);
		triprojected.p[2].x *= 0.5 * float(height); triprojected.p[2].y *= 0.5 * float(height);

		Point2d p1;
		Point2d p2;
		Point2d p3;

		p1.x = triprojected.p[0].x;
		p1.y = triprojected.p[0].y;

		p2.x = triprojected.p[1].x;
		p2.y = triprojected.p[1].y;

		p3.x = triprojected.p[2].x;
		p3.y = triprojected.p[2].y;

		result.push_back(p1);
		result.push_back(p2);
		result.push_back(p3);

		vector<Point2d> line1 = DDA(p1, p2);
		vector<Point2d> line2 = DDA(p2, p3);
		vector<Point2d> line3 = DDA(p1, p3);
		result.insert(result.end(), line1.begin(), line1.end());
		result.insert(result.end(), line2.begin(), line2.end());
		result.insert(result.end(), line3.begin(), line3.end());
	}
	return result;
}

int main(){
	vector <Point2d> p;
	const int width = 100;
	const int height = 45;
	string screen[width][height];
	int time = 0;
	float fTheta = 0;

	while (1) {
		system("cls");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				screen[j][i] = " ";
			}
		}
		time++;
		mat4x4 matRotX, matRotZ;
		fTheta = 1.0 * time;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cos(fTheta * 0.5);
		matRotX.m[1][2] = sin(fTheta * 0.5);
		matRotX.m[2][1] = -sin(fTheta * 0.5);
		matRotX.m[2][2] = cos(fTheta * 0.5);
		matRotX.m[3][3] = 1;

		p = Operate(matRotX, width, height);


		for (int i = 0; i < p.size(); i++) {
			int x_axis = (int)p[i].x;
			int y_axis = (int)p[i].y;
			if (screen[x_axis][y_axis] == " ") {
				screen[x_axis][y_axis] = ".";
			}
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				cout << screen[j][i];
			}
			cout << endl;
		}
	}

}


