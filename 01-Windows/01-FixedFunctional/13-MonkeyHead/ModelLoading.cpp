//Header
#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include <stdio.h>
#include<stdlib.h>
#include <cstdio>
#include "icon.h"

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 256
#define S_EQUAL 0

#define NR_POINT_COORDS			3
#define NR_TEXTURE_COORDS		2
#define NR_NORMAL_COORDS		3
#define NR_FACE_TOKENS			3
#define NR_TRIANGLE_VERTICES	3

#define MOKEYHEAD_X_TRANSLATE	0.0f
#define MOKEYHEAD_Y_TRANSLATE	0.0f
#define MOKEYHEAD_Z_TRANSLATE	-6.0f

#define MOKEYHEAD_X_SCALE_FACTOR	1.5f
#define MOKEYHEAD_Y_SCALE_FACTOR	1.5f
#define MOKEYHEAD_Z_SCALE_FACTOR	1.5f

#define START_ANGLE_POS		0.0f
#define END_ANGLE_POS		360.0f
#define MONKEYHEAD_ANGLE_INCREMENT	0.5f

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLU32.lib")

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);

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

/////////////////// MODEL LOADING VARIABLES //////////////////////
typedef struct vec_2d_int
{
	GLint **pp_arr;
	size_t size;
}vec_2d_int_t;

typedef struct vec_2d_float
{
	GLfloat **pp_arr;
	size_t size;
}vec_2d_float_t;

vec_2d_float *gp_vertices;
vec_2d_float *gp_texture;
vec_2d_float *gp_normals;

vec_2d_int_t *gp_face_tri, *gp_face_texture, *gp_face_normals;

FILE *g_fp_meshFile;

char g_line[BUFFER_SIZE];

GLfloat g_rotate;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Prototype
	void Initialize(void);
	void Display(void);
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
			fprintf(gpFile_rvg, "JAI HIND !\n");
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
	void Uninitialize(void);
	void LoadMeshData(void);

	gpFile_rvg = fopen("MonkeyHeadLoader.LOG","w");
	if (!gpFile_rvg)
	{
		Uninitialize();
	}

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	LoadMeshData();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void LoadMeshData(void)
{
	//Function Prototype
	void Uninitialize();

	//Variable Declaration
	vec_2d_int_t *create_vec_2d_int(void);
	vec_2d_float_t *create_vec_2d_float(void);

	void push_back_vec_2d_int(vec_2d_int_t *p_vec, int *p_arr);
	void push_back_vec_2d_float(vec_2d_float_t *p_vec, float *p_arr);

	void *xalloc(int, size_t);

	g_fp_meshFile = fopen("MonkeyHead.obj", "r");
	if (!g_fp_meshFile)
	{
		Uninitialize();
	}

	gp_vertices = create_vec_2d_float();
	gp_texture = create_vec_2d_float();
	gp_normals = create_vec_2d_float();

	gp_face_tri = create_vec_2d_int();
	gp_face_texture = create_vec_2d_int();
	gp_face_normals = create_vec_2d_int();

	char *sep_space = " ";
	char *sep_fslash = "/";

	char *first_token = NULL;
	char *token = NULL;
	char *face_tokens[NR_FACE_TOKENS];
	int nr_tokens;

	char *token_vertex_index = NULL;
	char *token_texture_index = NULL;
	char *token_normal_index = NULL;

	//Code
	while(fgets(g_line, BUFFER_SIZE, g_fp_meshFile) != NULL)
	{
		first_token = strtok(g_line, sep_space);
		if (strcmp(first_token, "v") == S_EQUAL)
		{
			GLfloat *pvec_point_coord = (GLfloat*)xalloc(NR_POINT_COORDS, sizeof(GLfloat));
			for (int i = 0; i != NR_POINT_COORDS; i++)
			{
				pvec_point_coord[i] = atof(strtok(NULL, sep_space));
			}
			push_back_vec_2d_float(gp_vertices, pvec_point_coord);
		}

		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			GLfloat *pvec_texture_coord = (GLfloat*)xalloc(NR_TEXTURE_COORDS, sizeof(GLfloat));
			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			{
				pvec_texture_coord[i] = atof(strtok(NULL, sep_space));
			}
			push_back_vec_2d_float(gp_texture, pvec_texture_coord);
		}

		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			GLfloat *pvec_normal_coord = (GLfloat*)xalloc(NR_NORMAL_COORDS, sizeof(GLfloat));
			for (int i = 0; i != NR_NORMAL_COORDS; i++)
			{
				pvec_normal_coord[i] = atof(strtok(NULL, sep_space));
			}
			push_back_vec_2d_float(gp_normals, pvec_normal_coord);
		}

		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			GLint *pvec_vertex_indices = (GLint*)xalloc(3, sizeof(GLint));
			GLint *pvec_texture_indices = (GLint*)xalloc(3, sizeof(GLint));
			GLint *pvec_normal_indices = (GLint*)xalloc(3, sizeof(GLint));

			memset((void*)face_tokens, 0, NR_FACE_TOKENS);

			nr_tokens = 0;
			while(token = strtok(NULL, sep_space))
			{
				if(strlen(token) < 3)
				{
					break;
				}
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i != NR_FACE_TOKENS; i++)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);

				pvec_vertex_indices[i] = atoi(token_vertex_index);
				pvec_texture_indices[i] = atoi(token_texture_index);
				pvec_normal_indices[i] = atoi(token_normal_index);
			}

			push_back_vec_2d_int(gp_face_tri, pvec_vertex_indices);
			push_back_vec_2d_int(gp_face_texture, pvec_texture_indices);
			push_back_vec_2d_int(gp_face_normals, pvec_normal_indices);
		}

		memset((void*)g_line, (int)'\0', BUFFER_SIZE);
	}


	fprintf(gpFile_rvg, "gp_vertices->size:%zu\ngp_texture->size:%zu\ngp_normals->size:%zu\ngp_face_tri->size:%zu\n",
	gp_vertices->size, gp_texture->size, gp_normals->size, gp_face_tri->size);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(MOKEYHEAD_X_TRANSLATE, MOKEYHEAD_Y_TRANSLATE, MOKEYHEAD_Z_TRANSLATE);
	glScalef(MOKEYHEAD_X_SCALE_FACTOR, MOKEYHEAD_Y_SCALE_FACTOR, MOKEYHEAD_Z_SCALE_FACTOR);

	glRotatef(g_rotate, 0.0f, 1.0f, 0.0f);

	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i != gp_face_tri->size; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (int j = 0; j != NR_TRIANGLE_VERTICES; j++)
		{
			int vi = gp_face_tri->pp_arr[i][j] - 1;
			glVertex3f(gp_vertices->pp_arr[vi][0], gp_vertices->pp_arr[vi][1], gp_vertices->pp_arr[vi][2]);
		}
		glEnd();
	}

	SwapBuffers(ghdc_rvg);
}

void Update(void)
{
	//Code
	g_rotate = g_rotate + MONKEYHEAD_ANGLE_INCREMENT;
	if (g_rotate >= END_ANGLE_POS)
	{
		g_rotate = START_ANGLE_POS;
	}
}

void Uninitialize()
{
	//Function Prototype
	void clean_vec_2d_int(vec_2d_int_t **pp_vec);
	void clean_vec_2d_float(vec_2d_float_t **pp_vec);
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

	if (g_fp_meshFile)
	{
		fclose(g_fp_meshFile);
		g_fp_meshFile = NULL;
	}	

	clean_vec_2d_float(&gp_vertices);
	clean_vec_2d_float(&gp_normals);
	clean_vec_2d_float(&gp_texture);

	clean_vec_2d_int(&gp_face_tri);
	clean_vec_2d_int(&gp_face_texture);
	clean_vec_2d_int(&gp_face_normals);
}

vec_2d_int_t *create_vec_2d_int(void)
{
	void *xalloc(int nr_element, size_t size_per_element);

	return(vec_2d_int_t*)xalloc(1, sizeof(vec_2d_int_t));
}

vec_2d_float_t *create_vec_2d_float(void)
{
	void *xalloc(int nr_element, size_t size_per_element);

	return(vec_2d_float_t*)xalloc(1, sizeof(vec_2d_float_t));
}

void push_back_vec_2d_int(vec_2d_int_t *p_vec, GLint *p_arr)
{
	void *xrealloc(void *p,  size_t new_size);

	p_vec->pp_arr = (GLint**)xrealloc(p_vec->pp_arr, (p_vec->size+1) * sizeof(int**));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size-1] = p_arr;
}

void push_back_vec_2d_float(vec_2d_float_t *p_vec, GLfloat *p_arr)
{
	void *xrealloc(void *p,  size_t new_size);

	p_vec->pp_arr = (GLfloat**)xrealloc(p_vec->pp_arr, (p_vec->size+1) * sizeof(GLfloat**));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size-1] = p_arr;
}

void clean_vec_2d_int(vec_2d_int_t **pp_vec)
{
	vec_2d_int_t *p_vec = *pp_vec;
	for (size_t i = 0; i != p_vec->size; i++)
	{
		free(p_vec->pp_arr[i]);
	}
	free(p_vec);
	*pp_vec = NULL;
}

void clean_vec_2d_float(vec_2d_float_t **pp_vec)
{
	vec_2d_float_t *p_vec = *pp_vec;
	for (size_t i = 0; i != p_vec->size; i++)
	{
		free(p_vec->pp_arr[i]);
	}
	free(p_vec);
	*pp_vec = NULL;
}

void *xalloc(int nr_elements, size_t size_per_element)
{
	void Uninitialize(void);

	void *p = calloc(nr_elements, size_per_element);
	if (!p)
	{
		fprintf(gpFile_rvg, "calloc:fatal:out of memory");
		Uninitialize();
	}
	return(p);
}

void *xrealloc(void *p, size_t new_size)
{
	void Uninitialize(void);
	void *ptr = realloc(p, new_size);
	if (!ptr)
	{
		fprintf(gpFile_rvg, "realloc:fatal:out of memory");
		Uninitialize();
	}
	return(ptr);
}







