// Pg1_raycast_zhu6.cpp : Define the entrance of program

#include "stdafx.h"
#include "string.h"
#include "Pg1_raycast_zhu6.h"
#include "tiny_obj_loader.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdio.h>
using namespace std;
using namespace calculate;
using namespace tinyobj;
//function declaration
void readWindow();
float inoutTriangle(struct dpoint point0, struct dpoint point1, struct dpoint point2,struct dpoint interp);
int lineNum(char file[]);

#define MAX_LOADSTRING 100
static HWND sHwnd;
//Color
static COLORREF redColor = RGB(255, 0, 0);
static COLORREF blueColor = RGB(0, 0, 255);
static COLORREF greenColor = RGB(0, 255, 0);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
struct coeff_plane_equa coeff2[10];				//uses to store the coefficients of 10 triangle plane
int A[6][6];									//store the sequence number of 2 triangles'6 points
struct dpoint p[6][4];							//store all points
std::vector<tinyobj::shape_t> shapes;
float mtl[6][3][3];								//store material properities	
COLORREF I;										//store light instensity
float N[6][3];									//store coeffients of normal vectors
const int Nline=5;
float Light[Nline][6];							//six light properities(position x,y,z; La, Ld, Ls)
float attenIndex;
float Window[2];								//the width and height of the window
dpoint viewPoint;								//eye position
float pa,pb,pc,ab,ac,bc,p1,p2,p3,p4;//the distance between point p and points of triangle
float sum,s1,s2,s3;//the area of triangles
float diff;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PG1_RAYCAST_ZHU6, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PG1_RAYCAST_ZHU6));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PG1_RAYCAST_ZHU6));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); //set the background color black
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PG1_RAYCAST_ZHU6);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	readWindow();
	viewPoint.x=0;
	viewPoint.y=0;
	viewPoint.z=-2;
	attenIndex=0.92*(lineNum("inputs/light.txt")+1);
	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, Window[0], Window[1], NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   //HDC hdc = GetDC(hWnd);
   //SetBkColor(hdc,RGB(0,0,0));
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////

//load OBJ file
static std::vector<tinyobj::shape_t> TestLoadObj(const char* filename, const char* basepath = NULL)
{
	std::cout << "Loading " << filename << std::endl;
	
	std::vector<tinyobj::shape_t> shapes;
	std::string err = tinyobj::LoadObj(shapes, filename, basepath);
	
	return shapes;
}

//Pixal in window coordinate
//Input: i, j; i means row number in 256x256, j means column number
//Output: the coordinate x1,y1 in 2x2 front plane
 struct vw convertPixel(int i,int j)
 {
	struct vw vw1;
	vw1.x1=-1+2.0*j/Window[0];
	vw1.y1=1-2.0*i/Window[1];
	return vw1;
 }

//parametric equation
//Input: coefficients of 2 points
//Output: six parameters of x,y,z parameter equations
struct coeff_para paraEquation(struct dpoint p1,struct dpoint p2)
{
	struct coeff_para coeff3;
	coeff3.x1=p1.x-p2.x; //obtain the coefficient of parameter equation
	coeff3.x2=p2.x;
	coeff3.y1=p1.y-p2.y;
	coeff3.y2=p2.y;
	coeff3.z1=p1.z-p2.z;
	coeff3.z2=p2.z;
	return coeff3;
}

//triangle plane equation
//Iutput: 3 vertice of triangle
//Output the coefficients of plane equation
struct coeff_plane_equa planeEquation(struct dpoint p1, struct dpoint p2,struct dpoint p3)
{
	struct coeff_plane_equa coeff6;
	coeff6.a = ( (p2.y-p1.y)*(p3.z-p1.z)-(p2.z-p1.z)*(p3.y-p1.y) );
    coeff6.b = ( (p2.z-p1.z)*(p3.x-p1.x)-(p2.x-p1.x)*(p3.z-p1.z) );
    coeff6.c = ( (p2.x-p1.x)*(p3.y-p1.y)-(p2.y-p1.y)*(p3.x-p1.x) );
    coeff6.d = ( 0-(coeff6.a*p1.x+coeff6.b*p1.y+coeff6.c*p1.z) );

    return coeff6;
}

//ray triangle-plane intersect point
//Iutput: the coefficients of parameter equations and coefficient of plane equation
//Output the intersect point
struct dpoint intersectPoint(struct dpoint point1, struct dpoint point2, struct coeff_plane_equa coeff2)
{
	struct dpoint intersectpoint;
	struct coefficient coeff1;
	coeff1.xt=point1.x-point2.x;
	coeff1.yt=point1.y-point2.y;
	coeff1.zt=point1.z-point2.z;
	float t=-(coeff2.d+coeff2.c*point2.z+coeff2.b*point2.y+coeff2.a*point2.x)/(coeff2.a*coeff1.xt+coeff2.b*coeff1.yt+coeff2.c*coeff1.zt);
	intersectpoint.x=point2.x+coeff1.xt*t;
	intersectpoint.y=point2.y+coeff1.yt*t;
	intersectpoint.z=point2.z+coeff1.zt*t;

	return intersectpoint;
}

//read the txt file line number
int lineNum(char file[])
{
	char c;
	int h=0;
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL)
	return -1;
	while((c=fgetc(fp))!=EOF)
	{
		if(c=='\n')
		h++;
		else
		{
			c=fgetc(fp);
			 if(c==EOF)
			 {
				 h++;
				 break;
			 }
		}
	}
	return h+1;
}

//calculate all triangles plane equations
void  readObj()
{
	shapes = TestLoadObj("inputs/cube.obj");
	//calculate all triangles plane equations
	for (size_t i = 0; i < shapes.size(); i++) {
		//every time have 4 points and use three of them making up triangle
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			p[i][v].x=shapes[i].mesh.positions[3*v+0];
			p[i][v].y=shapes[i].mesh.positions[3*v+1];
			p[i][v].z=shapes[i].mesh.positions[3*v+2];
		}
		//store the sequence number of 2 triangles'6 points
		for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
			A[i][f]=shapes[i].mesh.indices[f];
		}
		//calculate the plane equation of triangle
		coeff2[i]=planeEquation(p[i][A[i][0]],p[i][A[i][1]],p[i][A[i][2]]);
		coeff2[i+5]=coeff2[i];

		//calculate normal vectors
		//p[i][A[i][0]] and p[i][A[i][1]] makes vector a
		//p[i][A[i][1]] and p[i][A[i][3]] makes vector b
		// the direction of vector is important
		N[i][0]=-((p[i][A[i][0]].y-p[i][A[i][1]].y)*(p[i][A[i][0]].z-p[i][A[i][2]].z)-(p[i][A[i][0]].y-p[i][A[i][2]].y)*(p[i][A[i][0]].z-p[i][A[i][1]].z))/4; //coefficient of X,divided by 4 means normalized
		N[i][1]=-((p[i][A[i][0]].x-p[i][A[i][2]].x)*(p[i][A[i][0]].z-p[i][A[i][1]].z)-(p[i][A[i][0]].x-p[i][A[i][1]].x)*(p[i][A[i][0]].z-p[i][A[i][2]].z))/4; //coefficient of Y,divided by 4 means normalized
		N[i][2]=-((p[i][A[i][0]].x-p[i][A[i][1]].x)*(p[i][A[i][0]].y-p[i][A[i][2]].y)-(p[i][A[i][0]].x-p[i][A[i][2]].x)*(p[i][A[i][0]].y-p[i][A[i][1]].y))/4; //coefficient of Z,divided by 4 means normalized

		//store the material properities
		for(int r = 0; r < 3; r++) {
			mtl[i][0][r]=shapes[i].material.ambient[r];
			mtl[i][1][r]=shapes[i].material.diffuse[r];
			mtl[i][2][r]=shapes[i].material.specular[r];
		}
		
	}

}

//read light.txt file
void readLight()
{
	ifstream lightFile;
	lightFile.open("inputs/light.txt");
	char v[6];
	for (int i=0;i<Nline;i++)
	{
		for (int j=0;j<6;j++)
		{
			lightFile>>(float)Light[i][j];
		}
	}
	
	lightFile.close();
}


//read window.txt file
void readWindow()
{
	ifstream windowFile;
	windowFile.open("inputs/window.txt");
	char v[2];
	for (int j=0;j<2;j++)
	{
		windowFile>>(float)Window[j];
	}
	windowFile.close();
}

//calculate the light instensity
float lightInstensity(struct dpoint intersectpoint,int t) // t is group number
{
	float r=0.0,g=0.0,b=0.0; //representing ambient, diffuse and specular
	float temp=0.0;
	float L[4];//store the coefficients vector L, the fourth one is used to normalized
	float V[4];//store the coefficients vector V, the fourth one is used to normalized
	float R[4];//store the coefficients vector R, the fourth one is used to normalized
	struct coefficient coeff1;
	struct dpoint inpoint;
	struct dpoint lightpoint;
	bool shadow=false;
	int S=1;//Shadow index
	float lightIndex=0.92;//light intensity index

	for(int k=0;k<Nline;k++)
	{
		shadow=false;
		//I=LaKa+S*(LdKd(N*L)+LsKs(R*V))
		//Ambient
		r+=mtl[t][0][0]*max(Light[k][3],0)/attenIndex;
		g+=mtl[t][0][1]*max(Light[k][4],0)/attenIndex;
		b+=mtl[t][0][2]*max(Light[k][5],0)/attenIndex;
		
		//Shadow
		lightpoint.x=0.001+0.999*Light[k][0];
		lightpoint.y=Light[k][1];
		lightpoint.z=Light[k][2];

		//determine whether light in the box or not
		if(!((lightpoint.x>=-1)&&(lightpoint.x<=1) && (lightpoint.y>=-1)&&(lightpoint.y<=1) &&(lightpoint.z>=1)&&(lightpoint.z<=3)))
		{
			for(int t=0;t<5;t++)
			{
				inpoint=intersectPoint(lightpoint,intersectpoint,coeff2[t]);
				diff=inoutTriangle(p[t][0],p[t][1],p[t][2],inpoint);
				if(diff<0.001)
				{
					if(!(abs(inpoint.x-intersectpoint.x)<0.00001 &&abs(inpoint.x-intersectpoint.x)<0.00001 && abs(inpoint.x-intersectpoint.x)<0.00001))
					{
						shadow=true;
						break;
					}
				}
				diff=inoutTriangle(p[t][0],p[t][2],p[t][3],inpoint);
				if(diff<0.001)
				{
					if(!(abs(inpoint.x-intersectpoint.x)<0.00001 && abs(inpoint.x-intersectpoint.x)<0.00001&& abs(inpoint.x-intersectpoint.x)<0.00001))
					{
						shadow=true;
						break;
					}
				}
			}
		}

		if(shadow==true) {S=0;}
		else
		{
			S=1;
			//Diffuse
			//light at location (0,0.8,2),L is from the intersection point on the triangle to the light
			L[0]=intersectpoint.x-Light[k][0];
			L[1]=intersectpoint.y-Light[k][1];
			L[2]=intersectpoint.z-Light[k][2];
			L[3]=sqrt(pow(L[0],2)+pow(L[1],2)+pow(L[2],2));
			L[0]=L[0]/L[3];
			L[1]=L[1]/L[3];
			L[2]=L[2]/L[3];
			temp=S*mtl[t][1][0]*Light[k][3]*max((N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2]),0.0);
			r=r+temp;
			temp=S*mtl[t][1][1]*Light[k][4]*max((N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2]),0.0);
			g=g+temp;		
			temp=S*mtl[t][1][2]*Light[k][5]*max((N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2]),0.0);
			b=b+temp;

			//Specular
			//Locate the eye at (0 0 -2)
			V[0]=intersectpoint.x+1;
			V[1]=intersectpoint.y+1;
			V[2]=intersectpoint.z+2;
			V[3]=sqrt(pow(V[0],2)+pow(V[1],2)+pow(V[2],2));
			V[0]=V[0]/V[3];
			V[1]=V[1]/V[3];
			V[2]=V[2]/V[3];

			//R=2N(N¡¤L)-L
			R[0]=2*N[t][0]*(N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2])-L[0];
			R[1]=2*N[t][1]*(N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2])-L[1];
			R[2]=2*N[t][2]*(N[t][0]*L[0]+N[t][1]*L[1]+N[t][2]*L[2])-L[2];
			R[3]=sqrt(pow(R[0],2)+pow(R[1],2)+pow(R[2],2));
			R[0]=R[0]/R[3];
			R[1]=R[1]/R[3];
			R[2]=R[2]/R[3];
			temp=S*mtl[t][2][0]*Light[k][3]*pow(max((R[0]*V[0]+R[1]*V[1]+R[2]*V[2]),0),64);//N=500,  WILL BE DIVIDED BY 1000 AND MULTIPLIED BY 128, equals to 64
			r=min(r+temp,1);
			temp=S*mtl[t][2][1]*Light[k][4]*pow(max((R[0]*V[0]+R[1]*V[1]+R[2]*V[2]),0),64);
			g=min(g+temp,1);
			temp=S*mtl[t][2][2]*Light[k][5]*pow(max((R[0]*V[0]+R[1]*V[1]+R[2]*V[2]),0),64);
			b=min(b+temp,1);
		}
	}
	I=RGB(lightIndex*r*255,lightIndex*g*255,lightIndex*b*255);
	return I;
	
}

//draw color
void setPixel(int x, int y, COLORREF color)
{
	if (sHwnd == NULL)
	{		
		exit(0);
	}
	HDC hdc = GetDC(sHwnd);
	SetPixel(hdc, x, y, color);
	ReleaseDC(sHwnd, hdc);
	return;
}

//To determine whether a point in the triangle or not
float inoutTriangle(struct dpoint point0, struct dpoint point1, struct dpoint point2,struct dpoint interp)
{
	
	pa=sqrt(pow((interp.x-point0.x),2)+pow((interp.y-point0.y),2)+pow((interp.z-point0.z),2));
	pb=sqrt(pow((interp.x-point1.x),2)+pow((interp.y-point1.y),2)+pow((interp.z-point1.z),2));
	pc=sqrt(pow((interp.x-point2.x),2)+pow((interp.y-point2.y),2)+pow((interp.z-point2.z),2));
	ab=sqrt(pow((point0.x-point1.x),2)+pow((point0.y-point1.y),2)+pow((point0.z-point1.z),2));
	ac=sqrt(pow((point0.x-point2.x),2)+pow((point0.y-point2.y),2)+pow((point0.z-point2.z),2));
	bc=sqrt(pow((point1.x-point2.x),2)+pow((point1.y-point2.y),2)+pow((point1.z-point2.z),2));
	//Heron's formula
	p1=(ab+ac+bc)/2;
	sum=sqrt(p1*(p1-ab)*(p1-bc)*(p1-ac));
	p2=(pa+pb+ab)/2;
	s1=sqrt(p2*(p2-ab)*(p2-pa)*(p2-pb));
	p3=(pa+pc+ac)/2;
	s2=sqrt(p3*(p3-ac)*(p3-pa)*(p3-pc));
	p4=(pc+pb+bc)/2;
	s3=sqrt(p4*(p4-bc)*(p4-pc)*(p4-pb));
	return abs(sum-(s1+s2+s3));
}

void draw(int m,int n,struct dpoint interp)
{
	
	for (size_t i = 0; i < shapes.size(); i++) 
	{
		diff=inoutTriangle(p[i][A[i][0]], p[i][A[i][1]], p[i][A[i][2]],interp);

		if(diff<0.001)
		{
			setPixel(m,n,lightInstensity(interp,i));
			break;
		}

		diff=inoutTriangle(p[i][A[i][3]], p[i][A[i][4]], p[i][A[i][5]],interp);

		if(diff<0.001)
		{
			setPixel(m,n,lightInstensity(interp,i));
			break;
		}
	}
}

//calculate intersect point of all triangles and all rays 
void  interPoint()
{
	int i,j; //i means row number in view window, j means column number in view window
	struct vw vw1;
	struct dpoint temppoint;
	//calculate all triangles plane equations
	readObj();
	readLight();
	bool inside=false; //record the point position
	
	for(i=0; i<(int)Window[0]; i++)
	{
		for(j=0; j<(int)Window[1]; j++)
		{
			vw1=convertPixel(i,j);
			temppoint.x=vw1.x1; //obtain the coefficient of parameter equation
			temppoint.y=vw1.y1;
			temppoint.z=-1;
			struct dpoint intersectpoint;
			for(int k=0;k<10;k++)
			{
				intersectpoint=intersectPoint(temppoint,viewPoint,coeff2[k]);
				draw(j,i,intersectpoint);
			}
		}
	}
}

void SetWindowHandle(HWND hwnd)
{
	sHwnd = hwnd;
}


////////////////////////////////////////////////////////////////

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes message for the main window.
//
//  WM_COMMAND	- process the applcation menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
/////////////////////////////////////////////////////////////////////
//my code
	char *szHello = "SetPixel";
	RECT rt;
	int x = 0, y = 0, n = 0;
/////////////////////////////////////////////////////////////////////
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
/////////////////////////////////////////////////////////////////////////////
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any draw code...
		//get the dimensions of the window
		GetClientRect(hWnd, &rt);

		SetWindowHandle(hWnd);
		interPoint();
		break;
		EndPaint(hWnd, &ps);
		break;
//////////////////////////////////////////////////////////////////////////
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
