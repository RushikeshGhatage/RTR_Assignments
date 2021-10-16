//Header
#include<windows.h>
#include<stdlib.h>
#include<cstdio>
#include "Sphere.h"

#include<gl/GLEW.h>	//This file must be include before GL.h

#include<gl/GL.h>
#include "OGL.h"
#include "vmath.h"

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")

using namespace vmath;

enum 
{
	RVG_ATTRIBUTE_POSITION = 0,
	RVG_ATTRIBUTE_COLOR,
	RVG_ATTRIBUTE_NORMAL,
	RVG_ATTRIBUTE_TEXCOORD,
};

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

///////////// For Per Vertex /////////////
GLuint gVertexShaderObject_pv;
GLuint gFragmentShaderObject_pv;
GLuint gShaderProgramObject_pv;

GLuint LaUniform_pv, LdUniform_pv, LsUniform_pv;
GLuint KaUniform_pv, KdUniform_pv, KsUniform_pv;
GLint shininessUniform_pv;
GLuint lightPositionUniform_pv;

GLuint modelMatrixUniform_pv;
GLuint viewMatrixUniform_pv;
GLuint perspectiveProjectionUniform_pv;
GLuint LKeyPressedUniform_pv;

///////////// For Per Fragment /////////////
GLuint gVertexShaderObject_pf;
GLuint gFragmentShaderObject_pf;
GLuint gShaderProgramObject_pf;

GLuint LaUniform_pf, LdUniform_pf, LsUniform_pf;
GLuint KaUniform_pf, KdUniform_pf, KsUniform_pf;
GLint shininessUniform_pf;
GLuint lightPositionUniform_pf;

GLuint modelMatrixUniform_pf;
GLuint viewMatrixUniform_pf;
GLuint perspectiveProjectionUniform_pf;
GLuint LKeyPressedUniform_pf;

///////////// Common /////////////
GLuint vao_sphere;					//Vertex Array Objects
GLuint vbo_sphere_position;			//Vertex Buffer Objects
GLuint vbo_sphere_normal;			//For Normals
GLuint vbo_sphere_element;			//For Elements

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumVertices;
GLuint gNumElements;

bool bPerVertexLight = true;
bool bPerFragmentLight = false;

bool bLight;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};	//Grey Light
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	//White Light 
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

mat4 perspectiveProjectionMatrix;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Prototype
	void Initialize(void);
	void ToggleFullScreen(void);
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
	void Resize(int, int);
	void Uninitialize(void);
	void ToggleFullScreen(void);

	//Local Variable Declaration
	//Code
	switch (iMsg)
	{
	case WM_CREATE:
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

		case 'V':
		case 'v':
				bPerVertexLight = true;
				bPerFragmentLight = false;
			break;

		case 'F':
		case 'f':
				bPerFragmentLight = true;
				bPerVertexLight = false;
			break;

		case 'L':
		case 'l':
			if (bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
			}
			break;

		case 'J':
		case 'j':
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
	pfd_rvg.cRedBits = 8;
	pfd_rvg.cGreenBits = 8;
	pfd_rvg.cBlueBits = 8;
	pfd_rvg.cAlphaBits = 8;
	pfd_rvg.cDepthBits = 32;
	pfd_rvg.cColorBits = 32;

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

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc_rvg);
		ghrc_rvg = NULL;
		ReleaseDC(ghwnd_rvg, ghdc_rvg);
		ghdc_rvg = NULL;
	}

	///////////////////////////////////////// PER VERTEX /////////////////////////////////////////

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_pv = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode_pv = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition_pv;" \
									"in vec3 vNormal_pv;" \
									
									"uniform vec3 u_la_pv;" \
									"uniform vec3 u_ld_pv;" \
									"uniform vec3 u_ls_pv;" \
									"uniform vec4 u_light_position_pv;" \
									
									"uniform vec3 u_ka_pv;" \
									"uniform vec3 u_kd_pv;" \
									"uniform vec3 u_ks_pv;" \
									"uniform float u_materialShininess_pv;" \

									"uniform mat4 u_modelMatrix_pv;" \
									"uniform mat4 u_viewMatrix_pv;" \
									"uniform mat4 u_projectionMatrix_pv;" \

									"uniform int u_lKeyPressed_pv;" \

									"out vec3 fong_ads_light_pv;" \

									"void main(void)" \
									"{"	\
										"if(u_lKeyPressed_pv == 1)" \
										"{" \
											"vec4 eye_coordinates = (u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv);" \
											"vec3 transformed_normal_pv = normalize(mat3(u_viewMatrix_pv * u_modelMatrix_pv) * vNormal_pv);" \
											"vec3 light_direction_pv = normalize(vec3(u_light_position_pv - eye_coordinates));" \
											"vec3 reflection_vector = reflect(-light_direction_pv, transformed_normal_pv);" \
											"vec3 view_vector_pv = normalize(-eye_coordinates.xyz);" \
											"vec3 ambient = u_la_pv * u_ka_pv;" \
											"vec3 diffuse = u_ld_pv * u_kd_pv * max(dot(light_direction_pv, transformed_normal_pv), 0.0);" \
											"vec3 specular = u_ls_pv * u_ks_pv * pow(max(dot(reflection_vector, view_vector_pv), 0.0), u_materialShininess_pv);" \
											"fong_ads_light_pv = ambient + diffuse + specular;" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light_pv = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"gl_Position = u_projectionMatrix_pv * u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv;" \
									"}";		

	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_pv, 1, (const GLchar **) &VertexShaderCode_pv, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pv);	

	//Shader Compilation Error Checking
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	GLchar *szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject_pv, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pv, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Vertex Shader Compilation is done Successfully.\n");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pv = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pv = 
									"#version 460 core" \
									"\n" \
									"in vec3 fong_ads_light_pv;" \
									"out vec4 fragColor_pv;" \
									"void main(void)" \
									"{"	\
										"fragColor_pv = vec4(fong_ads_light_pv, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_pv, 1, (const GLchar **) &FragmentShaderCode_pv, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pv);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject_pv, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pv, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Fragment Shader Compilation is done Successfully.\n");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_pv = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_pv, gVertexShaderObject_pv);		//Vertex Shader
	glAttachShader(gShaderProgramObject_pv, gFragmentShaderObject_pv);		//Fragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_pv, RVG_ATTRIBUTE_POSITION, "vPosition_pv");
	glBindAttribLocation(gShaderProgramObject_pv, RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_pv);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject_pv, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_pv, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Program Shader Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	LaUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_la_pv");
	LdUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ld_pv");
	LsUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ls_pv");
	lightPositionUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_light_position_pv");

	KaUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ka_pv");
	KdUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_kd_pv");
	KsUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ks_pv");
	shininessUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_materialShininess_pv");

	LKeyPressedUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_lKeyPressed_pv");

	modelMatrixUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_modelMatrix_pv");
	viewMatrixUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_viewMatrix_pv");
	perspectiveProjectionUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_projectionMatrix_pv");

	///////////////////////////////////////// PER FRAGMENT /////////////////////////////////////////

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode_pf = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition_pf;" \
									"in vec3 vNormal_pf;" \

									"out vec3 transformed_normal_pf;" \
									"out vec3 light_direction_pf;" \
									"out vec3 view_vector_pf;" \
									
									"uniform vec4 u_light_position_pf;" \
									"uniform int u_lKeyPressed_pf;" \

									"uniform mat4 u_modelMatrix_pf;" \
									"uniform mat4 u_viewMatrix_pf;" \
									"uniform mat4 u_projectionMatrix_pf;" \

									"void main(void)" \
									"{"	\
										"if(u_lKeyPressed_pf == 1)" \
										"{" \
											"vec4 eye_coordinates = (u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf);" \
											"transformed_normal_pf = mat3(u_viewMatrix_pf * u_modelMatrix_pf) * vNormal_pf;" \
											"light_direction_pf = vec3(u_light_position_pf - eye_coordinates);" \
											"view_vector_pf = (-eye_coordinates.xyz);" \
										"}" \
										"gl_Position = u_projectionMatrix_pf * u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf;" \
									"}";	
	
	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_pf, 1, (const GLchar **) &VertexShaderCode_pf, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pf);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject_pf, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pf, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Vertex Shader Compilation is done Successfully.\n");
	}										

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pf = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pf = 
									"#version 460 core" \
									"\n" \
									"out vec4 fragColor_pf;" \

									"in vec3 transformed_normal_pf;" \
									"in vec3 light_direction_pf;" \
									"in vec3 view_vector_pf;" \
									
									"uniform vec3 u_la_pf;" \
									"uniform vec3 u_ld_pf;" \
									"uniform vec3 u_ls_pf;" \

									"uniform vec3 u_ka_pf;" \
									"uniform vec3 u_kd_pf;" \
									"uniform vec3 u_ks_pf;" \
									"uniform float u_materialShininess_pf;" \
									"out vec3 fong_ads_light_pf;" \

									"uniform int u_lKeyPressed_pf;" \

									"void main(void)" \
									"{"	\

									"if(u_lKeyPressed_pf == 1)" \
										"{" \
											"vec3 normalized_tranformed_normal = normalize(transformed_normal_pf);" \
											"vec3 normalized_light_direction = normalize(light_direction_pf);" \
											"vec3 normalized_view_vector = normalize(view_vector_pf);"
											"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" \
											"vec3 ambient = u_la_pf * u_ka_pf;" \
											"vec3 diffuse = u_ld_pf * u_kd_pf * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" \
											"vec3 specular = u_ls_pf * u_ks_pf * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess_pf);" \
											"fong_ads_light_pf = ambient + diffuse + specular;" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light_pf = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"fragColor_pf = vec4(fong_ads_light_pf, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_pf, 1, (const GLchar **) &FragmentShaderCode_pf, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pf);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject_pf, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pf, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Fragment Shader Compilation is done Successfully.\n");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_pf = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_pf, gVertexShaderObject_pf);		//Vertex Shader
	glAttachShader(gShaderProgramObject_pf, gFragmentShaderObject_pf);	//Gragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_pf, RVG_ATTRIBUTE_POSITION, "vPosition_pf");
	glBindAttribLocation(gShaderProgramObject_pf, RVG_ATTRIBUTE_NORMAL, "vNormal_pf");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_pf);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject_pf, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_pf, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Program Shader Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	LaUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_la_pf");
	LdUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ld_pf");
	LsUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ls_pf");
	lightPositionUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_light_position_pf");

	KaUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ka_pf");
	KdUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_kd_pf");
	KsUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ks_pf");
	shininessUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_materialShininess_pf");

	LKeyPressedUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_lKeyPressed_pf");

	modelMatrixUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_modelMatrix_pf");
	viewMatrixUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_viewMatrix_pf");
	perspectiveProjectionUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_projectionMatrix_pf");


	////////////////////////////////////////////////////////////////////////////////////////

	//Vertices Array Declaration
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	
	//Create Cassette to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);
			/////////////////////////// For POSITION ///////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_sphere_position);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			/////////////////////////// For NORMAL ///////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_sphere_normal);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_NORMAL);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			/////////////////////////// For ELEMENT ///////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_sphere_element);
 
			//Bind Buffer Array
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);

			//Fill Data into Buffer Array
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);

			//Unbind Buffer Array
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Stop Creating Cassette
	glBindVertexArray(0);
	
	//SetClearDepth
	//glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);		//Hidden Surface Removal

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	bLight = false;

	//Set Perspective Matrix to Identity
	perspectiveProjectionMatrix = mat4::identity();

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Display(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	if (bPerVertexLight == true)
	{
		glUseProgram(gShaderProgramObject_pv);
	}
	else
	{
		glUseProgram(gShaderProgramObject_pf);
	}

	if (bLight == true)
	{
		glUniform1i(LKeyPressedUniform_pv, 1);
		
		glUniform3fv(LaUniform_pv, 1, lightAmbient);
		glUniform3fv(LdUniform_pv, 1, lightDiffuse);
		glUniform3fv(LsUniform_pv, 1, lightSpecular);

		glUniform3fv(KaUniform_pv, 1, materialAmbient);
		glUniform3fv(KdUniform_pv, 1, materialDiffuse);
		glUniform3fv(KsUniform_pv, 1, materialSpecular);

		glUniform1f(shininessUniform_pv, materialShininess);
		glUniform4fv(lightPositionUniform_pv, 1, lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedUniform_pv, 0);
	}

	//OpenGL Drawing Code
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;

	//Set modelViewMatrix and projectionMatrix to Identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	modelMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	projectionMatrix = perspectiveProjectionMatrix;

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT

	glUniformMatrix4fv(modelMatrixUniform_pv, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_pv, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform_pv, 1, GL_FALSE, projectionMatrix);

	//Play Cassette
	glBindVertexArray(vao_sphere);

	//Draw Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Stop using Shader Program Object
	glUseProgram(0);
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

	if (vao_sphere)
	{
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}

	if (vbo_sphere_normal)
	{
		glDeleteBuffers(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
	}

	if (vbo_sphere_element)
	{
		glDeleteBuffers(1, &vbo_sphere_element);
		vbo_sphere_element = 0;
	}

	///////////////////////// Safe Shader Cleanup /////////////////////////
	if (gShaderProgramObject_pv)
	{
		glUseProgram(gShaderProgramObject_pv);
		
		GLsizei shaderCount;
		GLuint *pShader = NULL;
		glGetProgramiv(gShaderProgramObject_pv, GL_ATTACHED_SHADERS, &shaderCount);

		pShader = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(gShaderProgramObject_pv, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_pv, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		free(pShader);

		glDeleteProgram(gShaderProgramObject_pv);
		gShaderProgramObject_pv = 0;
		glUseProgram(0);
	}

}











