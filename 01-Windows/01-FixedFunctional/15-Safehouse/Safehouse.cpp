//Header
#include<windows.h>
#include<stdlib.h>
#include <cstdio>
#include<gl/GL.h>
#include<gl/GLU.h>
#include "icon.h"

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLU32.lib")

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBarrier(void);

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

GLfloat angle = 0.0f;
GLuint Texture_Door_Left;
GLuint Texture_Door_Right;
GLuint Texture_Wall;
GLuint Texture_Wall2;
GLuint Texture_Wall3;
GLuint Texture_Pillar;
GLuint Texture_Barrier;
GLuint Texture_Barrier2;
GLuint Texture_Barrier3;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Prototype
	void Initialize(void);
	void Display(void);
	void ToggleFullScreen(void);
	void Update(void);

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
	ToggleFullScreen();
	ShowWindow(hwnd_rvg, iCmdShow);

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
	pfd_rvg.cDepthBits = 24;

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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	loadTexture(&Texture_Door_Left, MAKEINTRESOURCE(TEXTURE_DOOR_LEFT));
	loadTexture(&Texture_Door_Right, MAKEINTRESOURCE(TEXTURE_DOOR_RIGHT));
	loadTexture(&Texture_Wall, MAKEINTRESOURCE(TEXTURE_WALL));
	loadTexture(&Texture_Wall2, MAKEINTRESOURCE(TEXTURE_WALL2));
	loadTexture(&Texture_Wall3, MAKEINTRESOURCE(TEXTURE_WALL3));
	loadTexture(&Texture_Pillar, MAKEINTRESOURCE(TEXTURE_PILLAR));
	loadTexture(&Texture_Barrier, MAKEINTRESOURCE(TEXTURE_BARRIER));
	loadTexture(&Texture_Barrier2, MAKEINTRESOURCE(TEXTURE_BARRIER2));
	loadTexture(&Texture_Barrier3, MAKEINTRESOURCE(TEXTURE_BARRIER3));
	
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
			
	//Code
	glTranslatef(0.0f, -0.3f, -2.5f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	/////////////////////////// Walls ///////////////////////////
	
	/////////////////////////// Front Wall ///////////////////////////
	////////////////// Right Front//////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wall);
	glColor3f(0.5f, 0.5f, 0.5f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.25f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.80f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.25f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.25f, 0.15f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.25f, 0.15f, 0.80f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, -0.10f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.25f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.25f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.25f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.80f);
	glEnd();

	////////////////// Left Front//////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wall);
	glColor3f(0.5f, 0.5f, 0.5f);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.80f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.25f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.25f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, 0.15f, 0.80f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.25f, -0.10f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.25f, -0.10f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.80f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.25f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.25f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.25f, -0.10f, 0.80f);
	glEnd();
		
	/////////////////////////// Back Wall ///////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wall3);
	glColor3f(0.5f, 0.5f, 0.5f);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.80f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.80f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, -0.10f, -0.80f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, -0.10f, -0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.80f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.80f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.80f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);
	glEnd();

	/////////////////////////// Left Wall ///////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wall2);
	glColor3f(0.5f, 0.5f, 0.5f);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, -0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, 0.15f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.10f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.75f, -0.10f, -0.75f);
	glEnd();
	
	/////////////////////////// Right Wall ///////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wall2);
	glColor3f(0.5f, 0.5f, 0.5f);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, 0.15f, 0.75f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.75f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.75f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.75f, -0.10f, -0.75f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, 0.15f, -0.75f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, 0.15f, 0.75f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.75f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, -0.75f);
	glEnd();

	/////////////////////////// Pillers ///////////////////////////
	////////////////// Left Front //////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Pillar);
	glColor3f(0.5f, 0.5f, 0.5f);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, 0.73f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, 0.20f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, 0.20f, 0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.82f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, 0.73f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, 0.82f);
	glEnd();

	////////////////// Left Back //////////////////


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, -0.73f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, 0.20f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, 0.20f, -0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.73f, -0.10f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.82f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.82f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.73f, -0.10f, -0.82f);
	glEnd();

	////////////////// Right Back //////////////////

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, -0.73f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, 0.20f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, 0.20f, -0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, -0.10f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, -0.10f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, -0.82f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, -0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, -0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, -0.82f);
	glEnd();

	////////////////// Right Front //////////////////

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.73f);
	glEnd();
	

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, 0.20f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, 0.20f, 0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, -0.10f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, -0.10f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.82f);
	glEnd();
	

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.73f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.82f);
	glEnd(); 
	

	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.82f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.82f, -0.10f, 0.82f);
	glEnd();
	
	////////////////// Center Right //////////////////

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.29f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.29f, -0.10f, 0.73f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.20f, 0.20f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.20f, 0.20f, 0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.29f, -0.10f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.29f, -0.10f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.20f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.20f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.82f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.29f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.29f, -0.10f, 0.82f);
	glEnd();
	
	////////////////// Center Left //////////////////

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.82f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.29f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.20f, 0.73f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.29f, -0.10f, 0.73f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, 0.20f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.20f, 0.20f, 0.82f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.29f, -0.10f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.29f, -0.10f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.29f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.29f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.29f, -0.10f, 0.82f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.20f, 0.20f, 0.82f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.20f, 0.73f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.73f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.20f, -0.10f, 0.82f);
	glEnd();
	
	/////////////////////////// Gates ///////////////////////////
	////////////////// Left Gate //////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Door_Left);
	glColor3f(0.937f, 0.325f, 0.313f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.005f, 0.18f, 0.78f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.18f, 0.78f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.09f, 0.78f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.005f, -0.09f, 0.78f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.005f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.09f, 0.77f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.005f, -0.09f, 0.77f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.005f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, 0.18f, 0.78f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.005f, 0.18f, 0.78f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.005f, -0.09f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.20f, -0.09f, 0.77f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.20f, -0.09f, 0.78f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.005f, -0.09f, 0.78f);
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.005f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.005f, 0.18f, 0.78f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.005f, -0.09f, 0.78f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.005f, -0.09f, 0.77f);
	glEnd();

	////////////////// Right Gate //////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Door_Right);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.20f, 0.18f, 0.78f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.18f, 0.68f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, -0.09f, 0.68f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.20f, -0.09f, 0.78f);
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.20f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.025f, 0.18f, 0.67f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.025f, -0.09f, 0.67f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.20f, -0.09f, 0.77f);
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.20f, 0.18f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.025f, 0.18f, 0.67f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, 0.18f, 0.68f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, 0.18f, 0.78f);
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.20f, -0.09f, 0.77f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.025f, -0.09f, 0.67f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, -0.09f, 0.68f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, -0.09f, 0.78f);
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.18f, 0.68f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.025f, 0.18f, 0.67f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.025f, -0.09f, 0.67f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.20f, -0.09f, 0.78f);
	glEnd();
	
	
	/////////////////////////// Barrier ///////////////////////////
		
	glTranslatef(-0.15f, 0.0f, 1.00f);	//Middle Front
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier);
	DrawBarrier();
	
	glTranslatef(0.15f, 0.0f, 0.0f);	//Left Front
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier2);
	DrawBarrier();	
	
	glTranslatef(0.15f, 0.0f, 0.0f);	//Right Front
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier3);
	DrawBarrier();
	
	glTranslatef(0.0f, 0.0f, -0.45f);	//Right Back
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier);
	DrawBarrier();

	glTranslatef(-0.15f, 0.0f, 0.0f);	//Middle Back
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier3);
	DrawBarrier();

	glTranslatef(-0.15f, 0.0f, 0.0f);	//Left Back
	glBindTexture(GL_TEXTURE_2D, Texture_Barrier2);
	DrawBarrier();
	
	angle += 0.01f;
	SwapBuffers(ghdc_rvg);
}

void Update()
{
	//Code
	if (angle >= 360)
	{
		angle = 0.0f;
	}
}

void DrawBarrier()
{
	glColor3f(0.70f, 0.70f, 0.70f);
	
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Base Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, -0.07f, 0.01f);	//A

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, -0.07f, 0.01f);	//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.065f, -0.10f, 0.02f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.065f, -0.10f, 0.02f);	//D
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Base Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, -0.07f, -0.01f);		//a

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, -0.07f, -0.01f);		//b

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.065f, -0.10f, -0.02f);	//c

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.065f, -0.10f, -0.02f);		//d
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Base Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.065f, -0.10f, -0.02f);		//d

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.065f, -0.10f, -0.02f);	//c

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.065f, -0.10f, 0.02f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.065f, -0.10f, 0.02f);		//D
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Base Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.06f, -0.07f, 0.01f);		//B

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, -0.07f, -0.01f);		//b

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.065f, -0.10f, -0.02f);	//c

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.065f, -0.10f, 0.02f);		//C
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Base Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, -0.07f, 0.01f);		//A

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.06f, -0.07f, -0.01f);		//a

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.065f, -0.10f, -0.02f);		//d

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.065f, -0.10f, 0.02f);		//D
	glEnd();



	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);			//Top Front
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, 0.01f);			//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, 0.0f, 0.01f);		//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.06f, -0.07f, 0.01f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.06f, -0.07f, 0.01f);		//A
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);			//Top Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, -0.01f);		//e

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, 0.0f, -0.01f);		//f

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.06f, -0.07f, -0.01f);		//b

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.06f, -0.07f, -0.01f);		//a
	glEnd();


	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, 0.01f);			//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, -0.01f);		//e

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.06f, 0.0f, -0.01f);		//f

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.06f, 0.0f, 0.01f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.06f, 0.0f, 0.01f);		//F

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.06f, 0.0f, -0.01f);		//f

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.06f, -0.07f, -0.01f);		//b

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.06f, -0.07f, 0.01f);		//B
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);			//Top Right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, 0.01f);			//E

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.06f, 0.0f, -0.01f);		//e

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.06f, -0.07f, -0.01f);		//a

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.06f, -0.07f, 0.01f);		//A
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

	if (Texture_Door_Left)
	{
		glDeleteTextures(1, &Texture_Door_Left);
	}
	
	if (Texture_Door_Right)
	{
		glDeleteTextures(1, &Texture_Door_Right);
	}

	if (Texture_Wall)
	{
		glDeleteTextures(1, &Texture_Wall);
	}

	if (Texture_Wall2)
	{
		glDeleteTextures(1, &Texture_Wall2);
	}

	if (Texture_Wall3)
	{
		glDeleteTextures(1, &Texture_Wall3);
	}

	if (Texture_Pillar)
	{
		glDeleteTextures(1, &Texture_Pillar);
	}

	if (Texture_Barrier)
	{
		glDeleteTextures(1, &Texture_Barrier);
	}

	if (Texture_Barrier2)
	{
		glDeleteTextures(1, &Texture_Barrier2);
	}

	if (Texture_Barrier3)
	{
		glDeleteTextures(1, &Texture_Barrier3);
	}
}











