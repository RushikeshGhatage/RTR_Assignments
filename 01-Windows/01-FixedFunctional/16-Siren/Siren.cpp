//Header
#include<windows.h>
#include<stdlib.h>
#include <cstdio>
#include<gl/GL.h>
#include<gl/GLU.h>
#include "icon.h"
#include<math.h>

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLU32.lib")

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void drawSpeaker(void);

//Global Variable Declaration
int x_rvg;
int y_rvg;

FILE* gpFile_rvg = NULL;

DWORD dwStyle_rvg;
WINDOWPLACEMENT wpPrev_rvg = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen_rvg = false;
HWND ghwnd_rvg = NULL;

bool gbActiveWindow_rvg = false;

HDC ghdc_rvg = NULL;
HGLRC ghrc_rvg = NULL;

GLUquadric* quadricCylinder = NULL;

GLuint Texture_Pole;
GLuint Texture_Base;
GLuint Texture_Box;
GLuint Texture_Speaker1;
GLuint Texture_Speaker2;
GLuint Texture_Back;

static GLfloat angle = 0.0f;
GLfloat radius_rvg = 0.05f;
GLfloat theta;
double PI = 3.14159;
GLfloat XPos_rvg = 0.0f;
GLfloat YPos_rvg = 0.0f;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Prototype
	void Initialize(void);
	void Display(void);
	void ToggleFullScreen(void);

	//Local Variable Declaration
	WNDCLASSEX wndclass_rvg;
	HWND hwnd_rvg;
	MSG msg_rvg;
	TCHAR szAppName_rvg[] = TEXT("MyApp");
	int XScreen_rvg;
	int YScreen_rvg;
	int midX1_rvg;
	int midY1_rvg;
	int midX2_rvg;
	int midY2_rvg;
	bool bDone_rvg = false;

	//Code

	//Centering of Window
	XScreen_rvg = GetSystemMetrics(SM_CXSCREEN);
	YScreen_rvg = GetSystemMetrics(SM_CYSCREEN);
	midX1_rvg = XScreen_rvg / 2;
	midY1_rvg = YScreen_rvg / 2;
	midX2_rvg = 800 / 2;
	midY2_rvg = 600 / 2;
	x_rvg = midX1_rvg - midX2_rvg;
	y_rvg = midY1_rvg - midY2_rvg;

	//FileIO
	if (fopen_s(&gpFile_rvg, "RVGLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot open Desired File !!!"), TEXT("Error"), MB_OK);
		exit(0);
	}

	//Initialization of WNDCLASSEX
	wndclass_rvg.cbSize = sizeof(WNDCLASSEX);
	wndclass_rvg.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass_rvg.cbClsExtra = 0;
	wndclass_rvg.cbWndExtra = 0;
	wndclass_rvg.lpfnWndProc = WndProc;
	wndclass_rvg.hInstance = hInstance;
	wndclass_rvg.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass_rvg.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass_rvg.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass_rvg.lpszClassName = szAppName_rvg;
	wndclass_rvg.lpszMenuName = NULL;
	wndclass_rvg.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	//Register above Class
	RegisterClassEx(&wndclass_rvg);

	//Create Window
	hwnd_rvg = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName_rvg,
		TEXT("Rushikesh Vinod Ghatage"),
		(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE),
		x_rvg,
		y_rvg,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd_rvg = hwnd_rvg;

	Initialize();
	ShowWindow(hwnd_rvg, iCmdShow);
	ToggleFullScreen();
	SetForegroundWindow(hwnd_rvg);
	SetFocus(hwnd_rvg);

	//Game Loop
	while (bDone_rvg == false)
	{
		if (PeekMessage(&msg_rvg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg_rvg.message == WM_QUIT)
			{
				bDone_rvg = true;
			}
			else
			{
				TranslateMessage(&msg_rvg);
				DispatchMessage(&msg_rvg);
			}
		}
		else
		{
			if (gbActiveWindow_rvg == true)
			{
				//Here You Should Call Update Function for OpenGL Rendering

				//Here You Should Call Display Function for OpenGL Rendering
				Display();
			}
		}
	}
	return((int)msg_rvg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd_rvg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Local Function Declaration
	void Display(void);
	void Resize(int, int);
	void Uninitialize(void);
	void ToggleFullScreen(void);

	//Local Variable Declaration


	//Code
	switch (iMsg)
	{
	case WM_CREATE:
		fprintf(gpFile_rvg, "INDIA is MY Country.\n");
		break;

	case WM_SETFOCUS:
		gbActiveWindow_rvg = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow_rvg = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd_rvg);
			break;

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd_rvg);
		break;

	case WM_DESTROY:
		fprintf(gpFile_rvg, "JAI HIND !\n");
		fclose(gpFile_rvg);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd_rvg, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	//Local Variable Declaration
	MONITORINFO mi_rvg = { sizeof(MONITORINFO) };

	//Code
	if (gbFullScreen_rvg == false)
	{
		dwStyle_rvg = GetWindowLong(ghwnd_rvg, GWL_STYLE);
		if (dwStyle_rvg & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd_rvg, &wpPrev_rvg) && GetMonitorInfo(MonitorFromWindow(ghwnd_rvg, MONITORINFOF_PRIMARY), &mi_rvg))
			{
				SetWindowLong(ghwnd_rvg, GWL_STYLE, (dwStyle_rvg & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd_rvg,
					HWND_TOP,
					mi_rvg.rcMonitor.left,
					mi_rvg.rcMonitor.top,
					mi_rvg.rcMonitor.right - mi_rvg.rcMonitor.left,
					mi_rvg.rcMonitor.bottom - mi_rvg.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(false);
		gbFullScreen_rvg = true;
	}

	else
	{
		SetWindowLong(ghwnd_rvg, GWL_STYLE, (dwStyle_rvg | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd_rvg, &wpPrev_rvg);
		ShowCursor(true);
		gbFullScreen_rvg = false;
	}
}

void Initialize(void)
{
	//Local Function Prototype
	void Resize(int, int);
	BOOL loadTexture(GLuint*, TCHAR[]);

	//Local Variable Declaration
	PIXELFORMATDESCRIPTOR pfd_rvg;
	int iPixelFormatIndex;

	//Code
	ghdc_rvg = GetDC(ghwnd_rvg);
	ZeroMemory(&pfd_rvg, sizeof(PIXELFORMATDESCRIPTOR));

	pfd_rvg.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd_rvg.nVersion = 1;
	pfd_rvg.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER);
	pfd_rvg.iPixelType = PFD_TYPE_RGBA;
	pfd_rvg.cColorBits = 32;
	pfd_rvg.cRedBits = 8;
	pfd_rvg.cGreenBits = 8;
	pfd_rvg.cBlueBits = 8;
	pfd_rvg.cAlphaBits = 8;
	//pfd_rvg.cDepthBits = 24;

	iPixelFormatIndex = ChoosePixelFormat(ghdc_rvg, &pfd_rvg);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile_rvg, "iPixelFormatIndex() Failed!");
		DestroyWindow(ghwnd_rvg);
	}

	if (SetPixelFormat(ghdc_rvg, iPixelFormatIndex, &pfd_rvg) == FALSE)
	{
		fprintf(gpFile_rvg, "SetPixelFormat() Failed!");
		DestroyWindow(ghwnd_rvg);
	}

	ghrc_rvg = wglCreateContext(ghdc_rvg);

	if (ghrc_rvg == NULL)
	{
		fprintf(gpFile_rvg, "wglCreateContext() Failed!");
		DestroyWindow(ghwnd_rvg);
	}

	if (wglMakeCurrent(ghdc_rvg, ghrc_rvg) == FALSE)
	{
		fprintf(gpFile_rvg, "wglMakeCurrent() Failed!");
		DestroyWindow(ghwnd_rvg);
	}

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	loadTexture(&Texture_Pole, MAKEINTRESOURCE(TEXTURE_POLE));
	loadTexture(&Texture_Base, MAKEINTRESOURCE(TEXTURE_BASE));
	loadTexture(&Texture_Box, MAKEINTRESOURCE(TEXTURE_BOX));
	loadTexture(&Texture_Speaker1, MAKEINTRESOURCE(TEXTURE_SPEAKER1));
	loadTexture(&Texture_Speaker2, MAKEINTRESOURCE(TEXTURE_SPEAKER2));
	loadTexture(&Texture_Back, MAKEINTRESOURCE(TEXTURE_BACK));


	Resize(WIN_WIDTH, WIN_HEIGHT);
}

BOOL loadTexture(GLuint* texture, TCHAR imageResource[])
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	bool bStatus = false;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		imageResource,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		fprintf(gpFile_rvg, "Image Load Successfully\n");
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glBindTexture(GL_TEXTURE_2D, 0);
		DeleteObject(hBitmap);
		bStatus = true;
	}
	return(bStatus);
}


void Resize(int width, int height)
{
	//Code
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Display(void)
{
	//Local Variable Declaration


	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	
	////////////////////////////////////// Speakers //////////////////////////////////////
		
	//////////////////////////////////// Front Speaker ////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.0f, 0.601f);
	drawSpeaker();
	
	//////////////////////////////////// Back Speaker ////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, -0.2f, -0.58f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
	drawSpeaker();

	//////////////////////////////////// Right Speaker ////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.45f, -0.4f, 0.00f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(50.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(50.0f, 0.0f, 0.0f, 1.0f);
	drawSpeaker();

	//////////////////////////////////// Left Speaker ////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(-1.5f, -4.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
	drawSpeaker();

	////////////////////////////////////// Pole //////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Pole);
	/////////////////// Front ///////////////////
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.1f, 0.15f);		//A

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.15f, 0.1f, 0.15f);	//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.15f, -4.0f, 0.15f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, -4.0f, 0.15f);	//D
	glEnd();

	/////////////////// Back ///////////////////
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.1f, -0.15f);	//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.15f, 0.1f, -0.15f);	//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.15f, -4.0f, -0.15f);	//G

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, -4.0f, -0.15f);	//H
	glEnd();

	/////////////////// Top ///////////////////
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.1f, -0.15f);	//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.15f, 0.1f, -0.15f);	//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.15f, 0.1f, 0.15f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, 0.1f, 0.15f);		//A
	glEnd();


	/////////////////// Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);
	
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.15f, 0.1f, 0.15f);	//B
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.15f, 0.1f, -0.15f);	//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.15f, -4.0f, -0.15f);	//G

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.15f, -4.0f, 0.15f);	//C
	glEnd();

	/////////////////// Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.1f, -0.15f);	//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.1f, 0.15f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, -4.0f, 0.15f);	//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, -4.0f, -0.15f);	//H
	glEnd();

	////////////////////////////////////// Box //////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.25f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Box);

	/////////////////// Front ///////////////////
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.13f, -2.5f, 0.40f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.13f, -2.5f, 0.40f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.13f, -3.0f, 0.25f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.13f, -3.0f, 0.25f);
	glEnd();

	/////////////////// Back ///////////////////
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.13f, -3.0f, 0.15f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.13f, -3.0f, 0.15f);
	glEnd();

	/////////////////// Top ///////////////////
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.13f, -2.5f, 0.40f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.13f, -2.5f, 0.40f);
	glEnd();

	/////////////////// Bottom ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.13f, -3.0f, 0.15f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.13f, -3.0f, 0.15f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.13f, -3.0f, 0.25f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.13f, -3.0f, 0.25f);
	glEnd();

	/////////////////// Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.13f, -2.5f, 0.40f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.13f, -3.0f, 0.15f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.13f, -3.0f, 0.25f);
	glEnd();

	/////////////////// Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.13f, -2.5f, 0.25f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.13f, -2.5f, 0.40f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.13f, -3.0f, 0.25f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.13f, -3.0f, 0.15f);
	glEnd();

	////////////////////////////////////// Base //////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 2.0f, -7.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Base);
	
	/////////////////// Front ///////////////////
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.30f, -4.0f, 0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.30f, -4.0f, 0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.30f, -4.20f, 0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.30f, -4.20f, 0.30f);
	glEnd();

	/////////////////// Back ///////////////////
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.30f, -4.0f, -0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.30f, -4.0f, -0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.30f, -4.20f, -0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.30f, -4.20f, -0.30f);
	glEnd();

	/////////////////// Top ///////////////////
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.30f, -4.0f, -0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.30f, -4.0f, -0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.30f, -4.0f, 0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.30f, -4.0f, 0.30f);
	glEnd();

	/////////////////// Bottom ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.30f, -4.20f, -0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.30f, -4.20f, -0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.30f, -4.20f, 0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.30f, -4.20f, 0.30f);
	glEnd();
	
	/////////////////// Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.30f, -4.0f, 0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.30f, -4.0f, -0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.30f, -4.20f, -0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.30f, -4.20f, 0.30f);
	glEnd();

	/////////////////// Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.30f, -4.0f, -0.30f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.30f, -4.0f, 0.30f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.30f, -4.20f, 0.30f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.30f, -4.20f, -0.30f);
	glEnd();
	
	angle += 0.03f;

	SwapBuffers(ghdc_rvg);
}

void drawSpeaker()
{
	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Back ///////////////////
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.35f);		//e

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.1f, 0.1f, -0.35f);	//f

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, -0.1f, -0.35f);	//g

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.1f, -0.1f, -0.35f);	//h
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Speaker1);
	/////////////////// Front Top ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.2f, 0.2f, 0.10f);		//A

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.25f);		//a

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, 0.1f, -0.25f);	//b

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, 0.2f, 0.10f);		//B
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Speaker1);
	/////////////////// Front Bottom ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.2f, -0.2f, 0.10f);		//D

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.1f, -0.1f, -0.25f);	//d

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, -0.1f, -0.25f);	//c

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, -0.2f, 0.10f);	//C
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Speaker2);
	/////////////////// Front Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, 0.2f, 0.10f);		//B

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.1f, 0.1f, -0.25f);	//b

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, -0.1f, -0.25f);	//c

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, -0.2f, 0.10f);	//C
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Speaker2);
	/////////////////// Front Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.2f, 0.2f, 0.10f);		//A

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.25f);		//a

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.1f, -0.1f, -0.25f);	//d

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.2f, -0.2f, 0.10f);		//D
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Back Top ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.25f);		//a

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.35f);		//e

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, 0.1f, -0.35f);	//f

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.1f, 0.1f, -0.25f);	//b
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Back Bottom ///////////////////
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.1f, -0.1f, -0.25f);	//d

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.1f, -0.1f, -0.35f);	//h

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.1f, -0.1f, -0.35f);	//g

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.1f, -0.1f, -0.25f);	//c
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Back Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.1f, 0.1f, -0.25f);	//b

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.1f, 0.1f, -0.35f);	//f

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, -0.1f, -0.35f);	//g

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.1f, -0.1f, -0.25f);	//c
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Back Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.25f);		//a

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.1f, 0.1f, -0.35f);		//e

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.1f, -0.1f, -0.35f);	//h

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.1f, -0.1f, -0.25f);	//d
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Handle Left ///////////////////
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.101f, 0.02f, -0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.101f, 0.02f, -0.45f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.101f, -0.02f, -0.45f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.101f, -0.02f, -0.30f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Handle Right ///////////////////
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.101f, 0.02f, -0.30f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.101f, 0.02f, -0.45f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.101f, -0.02f, -0.45f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.101f, -0.02f, -0.30f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture_Back);
	/////////////////// Handle Back ///////////////////
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.101f, 0.02f, -0.45f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.101f, 0.02f, -0.45f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.101f, -0.02f, -0.45f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.101f, -0.02f, -0.45f);
	glEnd();

	
	/////////////////// Inner Cyliner ///////////////////
	glTranslatef(0.0f, 0.0f, -0.3f);
	glColor3f(0.737f, 0.666f, 0.643f);
	quadricCylinder = gluNewQuadric();
	gluCylinder(quadricCylinder, 0.1f, 0.05f, 0.35f, 50, 50);


	/////////////////// Inner Circle ///////////////////
	glTranslatef(0.0f, 0.0f, 0.35f);
	glColor3f(0.647f, 0.364f, 0.207f);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= 360; i++)
	{
		theta = (GLfloat)(i * PI / 180);
		glVertex2f((GLfloat)(XPos_rvg + (radius_rvg + 0.00) * cosf(theta)), (GLfloat)(YPos_rvg + (radius_rvg + 0.00) * sinf(theta)));
	}
	glEnd();

}


void Uninitialize()
{
	//Code
	if (gbFullScreen_rvg == true)
	{
		dwStyle_rvg = GetWindowLong(ghwnd_rvg, GWL_STYLE);
		SetWindowLong(ghwnd_rvg, GWL_STYLE, (dwStyle_rvg | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd_rvg, &wpPrev_rvg);
		SetWindowPos(ghwnd_rvg,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghrc_rvg)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc_rvg)
	{
		wglDeleteContext(ghrc_rvg);
		ghrc_rvg = NULL;
	}

	if (ghdc_rvg)
	{
		ReleaseDC(ghwnd_rvg, ghdc_rvg);
		ghdc_rvg = NULL;
	}

	if (gpFile_rvg)
	{
		fclose(gpFile_rvg);
		gpFile_rvg = NULL;
	}

	if (Texture_Pole)
	{
		glDeleteTextures(1, &Texture_Pole);
	}

	if (Texture_Base)
	{
		glDeleteTextures(1, &Texture_Base);
	}

	if (Texture_Box)
	{
		glDeleteTextures(1, &Texture_Box);
	}

	if (Texture_Speaker1)
	{
		glDeleteTextures(1, &Texture_Speaker1);
	}

	if (Texture_Speaker2)
	{
		glDeleteTextures(1, &Texture_Speaker2);
	}

	if (Texture_Back)
	{
		glDeleteTextures(1, &Texture_Back);
	}
}


