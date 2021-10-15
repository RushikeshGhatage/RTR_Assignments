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
	void Update(void);
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
				Update();

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

	Resize(WIN_WIDTH, WIN_HEIGHT);
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
	//Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -7.5f);


	///////////////////////////////////////// I /////////////////////////////////////////
	

	////////////////////////// Upper I //////////////////////////

	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.00f, 0.50f, 0.00f);		//A
	
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.80f, 0.50f, 0.00f);		//B
	
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.80f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.80f, 0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F
	
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.80f, 0.30f, 0.00f);		//D

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.00f, 0.30f, 0.00f);		//C
	glEnd();
	
	
	glBegin(GL_TRIANGLES);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.00f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.00f, 0.30f, 0.00f);		//C
	glEnd();


	////////////////////////// Middle I //////////////////////////
	

	glBegin(GL_TRIANGLES);		//Top
	
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.30f, 0.30f, 0.00f);		//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-2.50f, 0.30f, 0.00f);		//B

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.50f, 0.30f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.50f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.50f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.30f, -0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.30f, 0.30f, 0.00f);		//A

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.30f, -0.30f, 0.00f);		//D
	glEnd();
	
	
	////////////////////////// Lower I //////////////////////////
	
	
	glBegin(GL_POLYGON);		//Top
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.00f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.80f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.80f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.80f, -0.50f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.80f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.00f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.00f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.00f, -0.50f, 0.00f);		//D
	glEnd();


	///////////////////////////////////////// N /////////////////////////////////////////
	

	////////////////////////// Left N //////////////////////////


	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.50f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.70f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-1.70f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-1.70f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.70f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.50f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-1.50f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-1.50f, -0.50f, 0.00f);		//D
	glEnd();
	

	////////////////////////// Middle N //////////////////////////


	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.90f, -0.20f, 0.00f);		//A

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-1.50f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-1.50f, 0.35f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.90f, -0.35f, 0.00f);		//F
	glEnd();

	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.90f, -0.35f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-1.50f, 0.35f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-1.50f, 0.20f, 0.00f);		//C

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.90f, -0.50f, 0.00f);		//D
	glEnd();


	////////////////////////// Right N //////////////////////////


	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.70f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.90f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.90f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.90f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.90f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.70f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-0.70f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-0.70f, -0.50f, 0.00f);		//D
	glEnd();

		
	///////////////////////////////////////// D /////////////////////////////////////////


	glBegin(GL_POLYGON);		//Outer Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.20f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.40f, 0.50f, 0.00f);		//C

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.125f, 0.4f, 0.0f);			//H
	glEnd();

	glBegin(GL_POLYGON);		//Inner Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.125f, 0.4f, 0.0f);			//H

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.05f, 0.30f, 0.00f);		//b
	glEnd();
	
	glBegin(GL_POLYGON);		//Outer Upper Left
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-0.40f, 0.50f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.40f, 0.00f, 0.00f);		//D

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J
	glEnd();

	
	glBegin(GL_POLYGON);		//Inner Upper Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.214f, 0.00f, 0.00f);		//d
	glEnd();


	glBegin(GL_POLYGON);		//Outer Lower Left
	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.40f, 0.00f, 0.0f);		//D

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.40f, -0.50f, 0.0f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K
	glEnd();


	glBegin(GL_POLYGON);		//Inner Lower Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.214f, 0.00f, 0.0f);		//d

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.307f, 0.00f, 0.0f);		//J

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.214f, -0.30f, 0.0f);		//e
	glEnd();


	glBegin(GL_POLYGON);		//Outer Bottom
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.40f, -0.50f, 0.0f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.20f, -0.50f, 0.0f);		//F
	glEnd();


	glBegin(GL_POLYGON);		//Inner Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.05f, -0.30f, 0.0f);		//f

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.214f, -0.30f, 0.0f);		//e

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L
	glEnd();
	
	
	glBegin(GL_POLYGON);		//Outer Lower Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.50f, 0.00f, 0.0f);			//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.20f, -0.50f, 0.0f);		//F
	glEnd();


	glBegin(GL_POLYGON);		//Inner Lower Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.25f, 0.00f, 0.0f);			//a
	
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.05f, -0.30f, 0.0f);		//f

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L
	glEnd();

	
	glBegin(GL_POLYGON);		//Outer Upper Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.20f, 0.50f, 0.0f);			//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.125f, 0.40f, 0.0f);		//H

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.50f, 0.00f, 0.0f);			//A
	glEnd();


	glBegin(GL_POLYGON);		//Inner Upper Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.125f, 0.40f, 0.0f);		//H

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.005f, 0.30f, 0.0f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.25f, 0.00f, 0.00f);		//a

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G
	glEnd();

	
	glBegin(GL_POLYGON);		//Inner
	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.25f, 0.00f, 0.00f);		//a

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.05f, 0.30f, 0.00f);		//b
	
	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(-0.214f, -0.30f, 0.00f);		//d

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.05f, -0.30f, 0.00f);		//e
	glEnd();
	

	///////////////////////////////////////// I /////////////////////////////////////////


	////////////////////////// Upper I //////////////////////////


	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.45f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.65f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.65f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.65f, 0.30f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.65f, 0.30f, 0.00f);		//C

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.45f, 0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.45f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.45f, 0.30f, 0.00f);		//D
	glEnd();


	////////////////////////// Middle I //////////////////////////
	
	
	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.15f, 0.30f, 0.00f);		//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.95f, 0.30f, 0.00f);		//B

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.95f, 0.30f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.95f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(0.95f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.15f, -0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.15f, 0.30f, 0.00f);		//A

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.15f, -0.30f, 0.00f);		//D
	glEnd();

	
	////////////////////////// Lower I //////////////////////////
	
	
	glBegin(GL_POLYGON);		//Top
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.45f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.65f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.65f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.65f, -0.50f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.65f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.45f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.45f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.45f, -0.50f, 0.00f);		//D
	glEnd();


	///////////////////////////////////////// A /////////////////////////////////////////
	

	////////////////////////// Left A //////////////////////////

	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(2.15f, 0.50f, 0.00f);		//F

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.05f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.65f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E

	glVertex3f(1.65f, -0.50f, 0.00f);		//C

	glVertex3f(1.85f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.25f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.15f, 0.50f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.85f, -0.50f, 0.00f);		//D
	glEnd();

	
	////////////////////////// Middle A //////////////////////////

	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.29f, 0.00f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.08f, 0.00f, 0.00f);		//B

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.27f, -0.055f, 0.00f);		//a
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.08f, 0.00f, 0.00f);		//B

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.00f, -0.20f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.305f, -0.146f, 0.00f);		//d

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.00f, -0.20f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.37f, -0.20f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.29f, 0.00f, 0.00f);		//A

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.27f, -0.055f, 0.00f);		//a

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.305f, -0.146f, 0.00f);		//d

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.37f, -0.20f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Inner
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.27f, -0.055f, 0.00f);		//a

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.305f, -0.146f, 0.00f);		//d
	glEnd();


	////////////////////////// Right A //////////////////////////
	

	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.21f, 0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.50f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(2.62f, -0.50f, 0.00f);		//F
	glEnd();
	

	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(2.30f, 0.45f, 0.00f);		//A

	glVertex3f(2.21f, 0.30f, 0.00f);		//B

	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.30f, 0.45f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(2.62f, -0.50f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.71f, -0.50f, 0.00f);		//D
	glEnd();


	SwapBuffers(ghdc_rvg);
}

void Update()
{
	//Code

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
}











