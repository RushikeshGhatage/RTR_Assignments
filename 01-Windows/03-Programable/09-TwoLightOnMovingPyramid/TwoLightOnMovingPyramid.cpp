//Header
#include<windows.h>
#include<stdlib.h>
#include<cstdio>

#include<gl/GLEW.h>	//This file must be include before GL.h

#include<gl/GL.h>
#include "OGL.h"
#include "vmath.h"

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")

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

struct Light_rvg
{
	vec4 lightAmbient_rvg;
	vec4 lightDiffuse_rvg;
	vec4 lightSpecular_rvg;
	vec4 lightPosition_rvg;
};
struct Light_rvg light_rvg[2];


FILE* gpFile_rvg = NULL;

DWORD dwStyle_rvg;
WINDOWPLACEMENT wpPrev_rvg = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen_rvg = false;
HWND ghwnd_rvg = NULL;

bool gbActiveWindow_rvg = false;

HDC ghdc_rvg = NULL;
HGLRC ghrc_rvg = NULL;

GLuint gVertexShaderObject_rvg;
GLuint gFragmentShaderObject_rvg;
GLuint gShaderProgramObject_rvg;

GLuint vao_pyramid;					//Vertex Array Objects
GLuint vbo_pyramid_position;		//Vertex Buffer Objects
GLuint vbo_pyramid_normal;			//For Normals

GLuint LaUniform0_rvg, LdUniform0_rvg, LsUniform0_rvg;
GLuint LaUniform1_rvg, LdUniform1_rvg, LsUniform1_rvg;
GLuint lightPositionUniform0_rvg, lightPositionUniform1_rvg;

GLuint KaUniform_rvg, KdUniform_rvg, KsUniform_rvg;
GLint shininessUniform_rvg;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint LKeyPressedUniform;

bool bLight_rvg;
bool bAnimate_rvg;

GLfloat materialAmbient_rvg[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse_rvg[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular_rvg[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess_rvg = 128.0f;

GLfloat PAngle_rvg = 0.0f;

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

		case 'A':
		case 'a':
			if (bAnimate_rvg == true)
			{
				bAnimate_rvg = false;
			}
			else
			{
				bAnimate_rvg = true;
			}
			break;

		case 'L':
		case 'l':
			if (bLight_rvg == true)
			{
				bLight_rvg = false;
			}
			else
			{
				bLight_rvg = true;
			}
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

	light_rvg[0].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightDiffuse_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightSpecular_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightPosition_rvg = vec4(2.0f, 0.0f, 0.0f, 1.0f);

	light_rvg[1].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[1].lightDiffuse_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light_rvg[1].lightSpecular_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light_rvg[1].lightPosition_rvg = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition;" \
									"in vec3 vNormal;" \
									"uniform int u_lKeyPressed;" \

									"uniform mat4 u_modelMatrix;" \
									"uniform mat4 u_viewMatrix;" \
									"uniform mat4 u_projectionMatrix;" \

									"uniform vec3 u_ka;" \
									"uniform vec3 u_kd;" \
									"uniform vec3 u_ks;" \
									"uniform float u_materialShininess;" \
									
									"uniform vec3 u_la[2];" \
									"uniform vec3 u_ld[2];" \
									"uniform vec3 u_ls[2];" \
									"uniform vec4 u_light_position[2];" \

									"out vec3 fong_ads_light;" \

									"void main(void)" \
									"{"	\
										"vec3 ambient[2];" \
										"vec3 diffuse[2];" \
										"vec3 specular[2];" \
										"vec3 light_direction[2];" \
										"vec3 reflection_vector[2];" \

										"if(u_lKeyPressed == 1)" \
										"{" \
											"vec4 eye_coordinates = (u_viewMatrix * u_modelMatrix * vPosition);" \
											"vec3 transformed_normal = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
											"vec3 view_vector = normalize(-eye_coordinates.xyz);" \

											"for (int i = 0; i < 2; i++)" \
											"{" \
												"ambient[i] = u_la[i] * u_ka;" \
												"light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" \
												"diffuse[i] = u_ld[i] * u_kd * max(dot(light_direction[i], transformed_normal), 0.0);" \
												"reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
												"specular[i] = u_ls[i] * u_ks * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess);" \
												"fong_ads_light = fong_ads_light + ambient[i] + diffuse[i] + specular[i];" \
											"}" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
									"}";		
/*
1) Calculate Eye Coordinate
2) Calculate Normal Matrix
3) Calculate Transformed Normals
4) Calculate S (Source of Light_rvg)
5) Calculate Diffuse light_rvg using Light_rvg Equation
*/

	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_rvg, 1, (const GLchar **) &VertexShaderCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_rvg);	

	//Shader Compilation Error Checking
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	GLchar *szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Vertex Shader Compilation is done Successfully.\n");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec3 fong_ads_light;" \
									"out vec4 fragColor;" \
									"void main(void)" \
									"{"	\
										"fragColor = vec4(fong_ads_light, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_rvg, 1, (const GLchar **) &FragmentShaderCode, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Fragment Shader Compilation is done Successfully.\n");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_rvg = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_rvg, gVertexShaderObject_rvg);		//Vertex Shader
	glAttachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);	//Gragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_rvg);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Program Shader Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	LaUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la[0]");
	LdUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[0]");
	LsUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[0]");
	lightPositionUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[0]");

	LaUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la[1]");
	LdUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[1]");
	LsUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[1]");
	lightPositionUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[1]");

	KaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ka");
	KdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_kd");
	KsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ks");
	shininessUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_materialShininess");

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_lKeyPressed");

	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_viewMatrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_projectionMatrix");

	//Vertices Array Declaration
	const GLfloat pyramidVertices[] = 
	{
		0.0f, 1.0f, 0.00f, 		//Apex
		-1.0f, -1.0f, 1.0f, 	//Front Left Bottom
		1.0f, -1.0f, 1.0f,		//Front Right Bottom
		
		0.0f, 1.0f, 0.0f,		//Apex
		1.0f, -1.0f, -1.0f,		//Back Right Bottom
		-1.0f, -1.0f, -1.0f,	//Back Left Bottom

		0.0f, 1.0f, 0.0f,		//Apex
		-1.0f, -1.0f, -1.0f,	//Left Left Bottom
		-1.0f, -1.0f, 1.0f,		//Left Right Bottom

		0.0f, 1.0f, 0.0f,		//Apex
		1.0f, -1.0f, 1.0f,		//Right Left Bottom
		1.0f, -1.0f, -1.0f  	//Right Right Bottom
	};

	const GLfloat pyramidNormal[] =
	{
		0.0f, 0.447214f, 0.894427f,		//Front
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		0.0f, 0.447214f, -0.894427f,	//Back
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,

		-0.894427f, 0.447214f, 0.0f,	//Left
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f,

		0.894427f, 0.447214f, 0.0f,		//Right
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f
	};
	
	//Create Cassette for PYRAMID to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_pyramid_position);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			/////////////////////////////// FOR NORMAL ///////////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_pyramid_normal);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormal), pyramidNormal, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_NORMAL);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Stop Creating Cassette for PYRAMID
	glBindVertexArray(0);

	//SetClearDepth
	//glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);		//Hidden Surface Removal

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	bLight_rvg = false;
	bAnimate_rvg = false;

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
	glUseProgram(gShaderProgramObject_rvg);

	if (bLight_rvg == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		
		glUniform3fv(LaUniform0_rvg, 1, light_rvg[0].lightAmbient_rvg);
		glUniform3fv(LdUniform0_rvg, 1, light_rvg[0].lightDiffuse_rvg);
		glUniform3fv(LsUniform0_rvg, 1, light_rvg[0].lightSpecular_rvg);
		glUniform4fv(lightPositionUniform0_rvg, 1, light_rvg[0].lightPosition_rvg);

		glUniform3fv(LaUniform1_rvg, 1, light_rvg[1].lightAmbient_rvg);
		glUniform3fv(LdUniform1_rvg, 1, light_rvg[1].lightDiffuse_rvg);
		glUniform3fv(LsUniform1_rvg, 1, light_rvg[1].lightSpecular_rvg);
		glUniform4fv(lightPositionUniform1_rvg, 1, light_rvg[1].lightPosition_rvg);

		glUniform3fv(KaUniform_rvg, 1, materialAmbient_rvg);
		glUniform3fv(KdUniform_rvg, 1, materialDiffuse_rvg);
		glUniform3fv(KsUniform_rvg, 1, materialSpecular_rvg);

		glUniform1f(shininessUniform_rvg, materialShininess_rvg);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//OpenGL Drawing Code
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 rotateMatrix;

	//Set modelViewMatrix and projectionMatrix to Identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotateMatrix = mat4::identity();

	translateMatrix = vmath::translate(0.0f, 0.0f, -4.5f);
	rotateMatrix = vmath::rotate(PAngle_rvg, 0.0f, 1.0f, 0.0f);

	modelMatrix = translateMatrix * rotateMatrix;
	projectionMatrix = perspectiveProjectionMatrix;

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

	//Play Cassette of Triangle
	glBindVertexArray(vao_pyramid);

	//Draw Object Triangle
	glDrawArrays(GL_TRIANGLES, 0, 12);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Stop using Shader Program Object
	glUseProgram(0);
	SwapBuffers(ghdc_rvg);
}

void Update()
{
	//Code
	if (bAnimate_rvg == true)
	{
		PAngle_rvg += 1.0f;

		if (PAngle_rvg >= 360.0f)
		{
			PAngle_rvg = 0.0f;
		}
	}
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

	if (vao_pyramid)
	{
		glDeleteVertexArrays(1, &vao_pyramid);
		vao_pyramid = 0;
	}

	if (vbo_pyramid_position)
	{
		glDeleteBuffers(1, &vbo_pyramid_position);
		vbo_pyramid_position = 0;
	}

	if (vbo_pyramid_normal)
	{
		glDeleteBuffers(1, &vbo_pyramid_normal);
		vbo_pyramid_normal = 0;
	}

	///////////////////////// Safe Shader Cleanup /////////////////////////
	if (gShaderProgramObject_rvg)
	{
		glUseProgram(gShaderProgramObject_rvg);
		
		GLsizei shaderCount;
		GLuint *pShader = NULL;
		glGetProgramiv(gShaderProgramObject_rvg, GL_ATTACHED_SHADERS, &shaderCount);

		pShader = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(gShaderProgramObject_rvg, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_rvg, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		free(pShader);

		glDeleteProgram(gShaderProgramObject_rvg);
		gShaderProgramObject_rvg = 0;
		glUseProgram(0);
	}

}











