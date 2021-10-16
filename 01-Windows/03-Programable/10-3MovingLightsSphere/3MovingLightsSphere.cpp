//Header
#include<windows.h>
#include<stdlib.h>
#include<cstdio>

#include<gl/GLEW.h>	//This file must be include before GL.h

#include<gl/GL.h>
#include "OGL.h"
#include "vmath.h"
#include "Sphere.h"

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

struct Light_rvg
{
	vec4 lightAmbient_rvg;
	vec4 lightDiffuse_rvg;
	vec4 lightSpecular_rvg;
	vec4 lightPosition_rvg;
};
struct Light_rvg light_rvg[3];


FILE* gpFile_rvg = NULL;

DWORD dwStyle_rvg;
WINDOWPLACEMENT wpPrev_rvg = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen_rvg = false;
HWND ghwnd_rvg = NULL;

bool gbActiveWindow_rvg = false;

HDC ghdc_rvg = NULL;
HGLRC ghrc_rvg = NULL;

////// For Per vertex //////
GLuint gVertexShaderObject_pv_rvg;
GLuint gFragmentShaderObject_pv_rvg;
GLuint gShaderProgramObject_pv_rvg;

GLuint LaUniform0_pv_rvg, LdUniform0_pv_rvg, LsUniform0_pv_rvg;
GLuint LaUniform1_pv_rvg, LdUniform1_pv_rvg, LsUniform1_pv_rvg;
GLuint LaUniform2_pv_rvg, LdUniform2_pv_rvg, LsUniform2_pv_rvg;
GLuint KaUniform_pv_rvg, KdUniform_pv_rvg, KsUniform_pv_rvg;
GLint shininessUniform_pv_rvg;
GLuint lightPositionUniform0_pv_rvg, lightPositionUniform1_pv_rvg, lightPositionUniform2_pv_rvg;

GLuint modelMatrixUniform_pv_rvg;
GLuint viewMatrixUniform_pv_rvg;
GLuint perspectiveProjectionUniform_pv_rvg;
GLuint LKeyPressedUniform_pv_rvg;

////// For Per fragment //////
GLuint gVertexShaderObject_pf_rvg;
GLuint gFragmentShaderObject_pf_rvg;
GLuint gShaderProgramObject_pf_rvg;

GLuint LaUniform0_pf_rvg, LdUniform0_pf_rvg, LsUniform0_pf_rvg;
GLuint LaUniform1_pf_rvg, LdUniform1_pf_rvg, LsUniform1_pf_rvg;
GLuint LaUniform2_pf_rvg, LdUniform2_pf_rvg, LsUniform2_pf_rvg;
GLuint KaUniform_pf_rvg, KdUniform_pf_rvg, KsUniform_pf_rvg;
GLint shininessUniform_pf_rvg;
GLuint lightPositionUniform0_pf_rvg, lightPositionUniform1_pf_rvg, lightPositionUniform2_pf_rvg;

GLuint modelMatrixUniform_pf_rvg;
GLuint viewMatrixUniform_pf_rvg;
GLuint perspectiveProjectionUniform_pf_rvg;
GLuint LKeyPressedUniform_pf_rvg;

////// Common //////
GLuint vao_sphere_rvg;					//Vertex Array Objects
GLuint vbo_sphere_position_rvg;		//Vertex Buffer Objects
GLuint vbo_sphere_normal_rvg;			//For Normals
GLuint vbo_sphere_element_rvg;			//For Elements

GLfloat lightAngle0_rvg = 0.0f;
GLfloat lightAngle1_rvg = 0.0f;
GLfloat lightAngle2_rvg = 0.0f;

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumVertices;
GLuint gNumElements;

bool bPerVertexLight_rvg = true;
bool bPerFragmentLight_rvg = false;

bool bLight_rvg;

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

		case 'V':
		case 'v':
				bPerVertexLight_rvg = true;
				bPerFragmentLight_rvg = false;
			break;

		case 'F':
		case 'f':
				bPerFragmentLight_rvg = true;
				bPerVertexLight_rvg = false;
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

	light_rvg[0].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightDiffuse_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightSpecular_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[0].lightPosition_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	light_rvg[1].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[1].lightDiffuse_rvg = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	light_rvg[1].lightSpecular_rvg = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	light_rvg[1].lightPosition_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	light_rvg[2].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light_rvg[2].lightDiffuse_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light_rvg[2].lightSpecular_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light_rvg[2].lightPosition_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);	

	///////////////////////////////////////// PER VERTEX /////////////////////////////////////////

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_pv_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode_pv = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition_pv;" \
									"in vec3 vNormal_pv;" \

									"uniform vec3 u_la_pv[3];" \
									"uniform vec3 u_ld_pv[3];" \
									"uniform vec3 u_ls_pv[3];" \
									"uniform vec4 u_light_position_pv[3];" \

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
										"vec3 ambient[3];" \
										"vec3 diffuse[3];" \
										"vec3 specular[3];" \
										"vec3 light_direction[3];" \
										"vec3 reflection_vector[3];" \

										"if(u_lKeyPressed_pv == 1)" \
										"{" \
											"vec4 eye_coordinates = (u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv);" \
											"vec3 transformed_normal = normalize(mat3(u_viewMatrix_pv * u_modelMatrix_pv) * vNormal_pv);" \
											"vec3 view_vector = normalize(-eye_coordinates.xyz);" \

											"for (int i = 0; i < 3; i++)" \
											"{" \
												"ambient[i] = u_la_pv[i] * u_ka_pv;" \
												"light_direction[i] = normalize(vec3(u_light_position_pv[i] - eye_coordinates));" \
												"diffuse[i] = u_ld_pv[i] * u_kd_pv * max(dot(light_direction[i], transformed_normal), 0.0);" \
												"reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
												"specular[i] = u_ls_pv[i] * u_ks_pv * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess_pv);" \
												"fong_ads_light_pv = fong_ads_light_pv + ambient[i] + diffuse[i] + specular[i];" \
											"}" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light_pv = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"gl_Position = u_projectionMatrix_pv * u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv;" \
									"}";		
	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_pv_rvg, 1, (const GLchar **) &VertexShaderCode_pv, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pv_rvg);	

	//Shader Compilation Error Checking
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	GLchar *szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pv_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX : Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX : Vertex Shader Compilation is done Successfully.\n");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pv_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pv = 
									"#version 460 core" \
									"\n" \
									"in vec3 fong_ads_light_pv;" \
									"out vec4 fragColor;" \
									"void main(void)" \
									"{"	\
										"fragColor = vec4(fong_ads_light_pv, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_pv_rvg, 1, (const GLchar **) &FragmentShaderCode_pv, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pv_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_pv_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX : Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX : Fragment Shader Compilation is done Successfully.\n");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_pv_rvg = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_pv_rvg, gVertexShaderObject_pv_rvg);		//Vertex Shader
	glAttachShader(gShaderProgramObject_pv_rvg, gFragmentShaderObject_pv_rvg);	//Gragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_POSITION, "vPosition_pv");
	glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_pv_rvg);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject_pv_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_pv_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER VERTEX : Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX : Program Shader Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	LaUniform0_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_la_pv[0]");
	LdUniform0_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ld_pv[0]");
	LsUniform0_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ls_pv[0]");
	lightPositionUniform0_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_light_position_pv[0]");

	LaUniform1_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_la_pv[1]");
	LdUniform1_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ld_pv[1]");
	LsUniform1_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ls_pv[1]");
	lightPositionUniform1_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_light_position_pv[1]");

	LaUniform2_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_la_pv[2]");
	LdUniform2_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ld_pv[2]");
	LsUniform2_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ls_pv[2]");
	lightPositionUniform2_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_light_position_pv[2]");	

	KaUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ka_pv");
	KdUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_kd_pv");
	KsUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ks_pv");
	shininessUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_materialShininess_pv");

	LKeyPressedUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_lKeyPressed_pv");

	modelMatrixUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_modelMatrix_pv");
	viewMatrixUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_viewMatrix_pv");
	perspectiveProjectionUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_projectionMatrix_pv");

	///////////////////////////////////////// PER FRAGMENT /////////////////////////////////////////
	
	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_pf_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode_pf = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition_pf;" \
									"in vec3 vNormal_pf;" \

									"out vec3 transformed_normal_pf;" \
									"out vec3 light_direction_pf[3];" \
									"out vec3 view_vector_pf;" \

									"uniform vec4 u_light_position_pf[3];" \
									"uniform int u_lKeyPressed_pf;" \

									"uniform mat4 u_modelMatrix_pf;" \
									"uniform mat4 u_viewMatrix_pf;" \
									"uniform mat4 u_projectionMatrix_pf;" \
																
									"void main(void)" \
									"{"	\
										"if(u_lKeyPressed_pf == 1)" \
										"{" \
											"vec4 eye_coordinates_pf = (u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf);" \
											"transformed_normal_pf = mat3(u_viewMatrix_pf * u_modelMatrix_pf) * vNormal_pf;" \
											"view_vector_pf = -eye_coordinates_pf.xyz;" \

											"for (int i = 0; i < 3; i++)" \
											"{" \
												"light_direction_pf[i] = vec3(u_light_position_pf[i] - eye_coordinates_pf);" \
											"}" \
										"}" \
										"gl_Position = u_projectionMatrix_pf * u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf;" \
									"}";		
	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_pf_rvg, 1, (const GLchar **) &VertexShaderCode_pf, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pf_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_pf_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT : Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT : Vertex Shader Compilation is done Successfully.\n");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pf_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pf = 
									"#version 460 core" \
									"\n" \
									"out vec4 fragColor_pf;" \

									"in vec3 transformed_normal_pf;" \
									"in vec3 light_direction_pf[3];" \
									"in vec3 view_vector_pf;" \

									"uniform vec3 u_la_pf[3];" \
									"uniform vec3 u_ld_pf[3];" \
									"uniform vec3 u_ls_pf[3];" \
									
									"uniform vec3 u_ka_pf;" \
									"uniform vec3 u_kd_pf;" \
									"uniform vec3 u_ks_pf;" \
									"uniform float u_materialShininess_pf;" \

									"uniform int u_lKeyPressed_pf;" \

									"void main(void)" \
									"{"	\
										"vec3 ambient_pf[3];" \
										"vec3 diffuse_pf[3];" \
										"vec3 specular_pf[3];" \
										"vec3 reflection_vector_pf[3];" \
										"vec3 fong_ads_light_pf;" \

										"if(u_lKeyPressed_pf == 1)" \
										"{" \
											"vec3 normalized_light_direction_pf[3];" \
											"vec3 normalized_tranformed_normal_pf = normalize(transformed_normal_pf);" \
											"vec3 normalized_view_vector_pf = normalize(view_vector_pf);"
											
											"for (int i = 0; i < 3; i++)" \
											"{" \
												"reflection_vector_pf[i] = reflect(-normalized_light_direction_pf[i], normalized_tranformed_normal_pf);" \
												"normalized_light_direction_pf[i] = normalize(light_direction_pf[i]);" \

												"ambient_pf[i] = u_la_pf[i] * u_ka_pf;" \
												"diffuse_pf[i] = u_ld_pf[i] * u_kd_pf * max(dot(normalized_light_direction_pf[i], normalized_tranformed_normal_pf), 0.0);" \
												"specular_pf[i] = u_ls_pf[i] * u_ks_pf * pow(max(dot(reflection_vector_pf[i], normalized_view_vector_pf), 0.0), u_materialShininess_pf);" \
												"fong_ads_light_pf = fong_ads_light_pf + ambient_pf[i] + diffuse_pf[i] + specular_pf[i];" \
											"}"\
										"}"\
										"else" \
										"{" \
											"fong_ads_light_pf = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"fragColor_pf = vec4(fong_ads_light_pf, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_pf_rvg, 1, (const GLchar **) &FragmentShaderCode_pf, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pf_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_pf_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT : Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT : Fragment Shader Compilation is done Successfully.\n");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_pf_rvg = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_pf_rvg, gVertexShaderObject_pf_rvg);		//Vertex Shader
	glAttachShader(gShaderProgramObject_pf_rvg, gFragmentShaderObject_pf_rvg);	//Gragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_pf_rvg, RVG_ATTRIBUTE_POSITION, "vPosition_pf");
	glBindAttribLocation(gShaderProgramObject_pf_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal_pf");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_pf_rvg);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject_pv_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_pf_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "PER FRAGMENT : Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT : Program Shader Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	LaUniform0_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_la_pf[0]");
	LdUniform0_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ld_pf[0]");
	LsUniform0_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ls_pf[0]");
	lightPositionUniform0_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_light_position_pf[0]");

	LaUniform1_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_la_pf[1]");
	LdUniform1_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ld_pf[1]");
	LsUniform1_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ls_pf[1]");
	lightPositionUniform1_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_light_position_pf[1]");

	LaUniform2_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_la_pf[2]");
	LdUniform2_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ld_pf[2]");
	LsUniform2_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ls_pf[2]");
	lightPositionUniform2_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_light_position_pf[2]");	

	KaUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ka_pf");
	KdUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_kd_pf");
	KsUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ks_pf");
	shininessUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_materialShininess_pf");

	LKeyPressedUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_lKeyPressed_pf");

	modelMatrixUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_modelMatrix_pf");
	viewMatrixUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_viewMatrix_pf");
	perspectiveProjectionUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_projectionMatrix_pf");	

	////////////////////////////////////////////////////////////////////////////////////////

	//Vertices Array Declaration
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	
	//Create Cassette to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_sphere_rvg);
	glBindVertexArray(vao_sphere_rvg);
			/////////////////////////// For POSITION ///////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_sphere_position_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position_rvg);

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
			glGenBuffers(1, &vbo_sphere_normal_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal_rvg);

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
			glGenBuffers(1, &vbo_sphere_element_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);

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
	if (bPerVertexLight_rvg == true)
	{
		glUseProgram(gShaderProgramObject_pv_rvg);
	}
	else
	{
		glUseProgram(gShaderProgramObject_pf_rvg);
	}

	if (bLight_rvg == true)
	{
		light_rvg[0].lightPosition_rvg[0] = 0.0f;
		light_rvg[0].lightPosition_rvg[1] = 28.0f * sin(lightAngle0_rvg);
		light_rvg[0].lightPosition_rvg[2] = 28.0f * cos(lightAngle0_rvg);
		light_rvg[0].lightPosition_rvg[3] = 1.0f;

		light_rvg[1].lightPosition_rvg[0] = 28.0f * sin(lightAngle1_rvg);
		light_rvg[1].lightPosition_rvg[1] = 0.0f;
		light_rvg[1].lightPosition_rvg[2] = 28.0f * cos(lightAngle1_rvg);
		light_rvg[1].lightPosition_rvg[3] = 1.0f;

		light_rvg[2].lightPosition_rvg[0] = 28.0f * sin(lightAngle2_rvg);
		light_rvg[2].lightPosition_rvg[1] = 28.0f * cos(lightAngle2_rvg);
		light_rvg[2].lightPosition_rvg[2] = 0.0f;
		light_rvg[2].lightPosition_rvg[3] = 1.0f;

		if (bPerVertexLight_rvg == true)
		{

			glUniform1i(LKeyPressedUniform_pv_rvg, 1);
				
			glUniform3fv(LaUniform0_pv_rvg, 1, light_rvg[0].lightAmbient_rvg);
			glUniform3fv(LdUniform0_pv_rvg, 1, light_rvg[0].lightDiffuse_rvg);
			glUniform3fv(LsUniform0_pv_rvg, 1, light_rvg[0].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform0_pv_rvg, 1, light_rvg[0].lightPosition_rvg);

			glUniform3fv(LaUniform1_pv_rvg, 1, light_rvg[1].lightAmbient_rvg);
			glUniform3fv(LdUniform1_pv_rvg, 1, light_rvg[1].lightDiffuse_rvg);
			glUniform3fv(LsUniform1_pv_rvg, 1, light_rvg[1].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform1_pv_rvg, 1, light_rvg[1].lightPosition_rvg);

			glUniform3fv(LaUniform2_pv_rvg, 1, light_rvg[2].lightAmbient_rvg);
			glUniform3fv(LdUniform2_pv_rvg, 1, light_rvg[2].lightDiffuse_rvg);
			glUniform3fv(LsUniform2_pv_rvg, 1, light_rvg[2].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform2_pv_rvg, 1, light_rvg[2].lightPosition_rvg);

			glUniform3fv(KaUniform_pv_rvg, 1, materialAmbient_rvg);
			glUniform3fv(KdUniform_pv_rvg, 1, materialDiffuse_rvg);
			glUniform3fv(KsUniform_pv_rvg, 1, materialSpecular_rvg);

			glUniform1f(shininessUniform_pv_rvg, materialShininess_rvg);
		}
			
		else if(bPerFragmentLight_rvg == true)
		{
			glUniform1i(LKeyPressedUniform_pf_rvg, 1);
				
			glUniform3fv(LaUniform0_pf_rvg, 1, light_rvg[0].lightAmbient_rvg);
			glUniform3fv(LdUniform0_pf_rvg, 1, light_rvg[0].lightDiffuse_rvg);
			glUniform3fv(LsUniform0_pf_rvg, 1, light_rvg[0].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform0_pf_rvg, 1, light_rvg[0].lightPosition_rvg);

			glUniform3fv(LaUniform1_pf_rvg, 1, light_rvg[1].lightAmbient_rvg);
			glUniform3fv(LdUniform1_pf_rvg, 1, light_rvg[1].lightDiffuse_rvg);
			glUniform3fv(LsUniform1_pf_rvg, 1, light_rvg[1].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform1_pf_rvg, 1, light_rvg[1].lightPosition_rvg);

			glUniform3fv(LaUniform2_pf_rvg, 1, light_rvg[2].lightAmbient_rvg);
			glUniform3fv(LdUniform2_pf_rvg, 1, light_rvg[2].lightDiffuse_rvg);
			glUniform3fv(LsUniform2_pf_rvg, 1, light_rvg[2].lightSpecular_rvg);
			glUniform4fv(lightPositionUniform2_pf_rvg, 1, light_rvg[2].lightPosition_rvg);

			glUniform3fv(KaUniform_pf_rvg, 1, materialAmbient_rvg);
			glUniform3fv(KdUniform_pf_rvg, 1, materialDiffuse_rvg);
			glUniform3fv(KsUniform_pf_rvg, 1, materialSpecular_rvg);

			glUniform1f(shininessUniform_pf_rvg, materialShininess_rvg);
		}
	}

	else
	{
		glUniform1i(LKeyPressedUniform_pv_rvg, 0);
		glUniform1i(LKeyPressedUniform_pf_rvg, 0);
	}

	//OpenGL Drawing Code
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;

	//Set modelViewMatrix and projectionMatrix to Identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();

	translateMatrix = vmath::translate(0.0f, 0.0f, -2.5f);

	modelMatrix = translateMatrix;
	projectionMatrix = perspectiveProjectionMatrix;

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT

	if (bPerVertexLight_rvg == true)
	{
		glUniformMatrix4fv(modelMatrixUniform_pv_rvg, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_pv_rvg, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(perspectiveProjectionUniform_pv_rvg, 1, GL_FALSE, projectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelMatrixUniform_pf_rvg, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_pf_rvg, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(perspectiveProjectionUniform_pf_rvg, 1, GL_FALSE, projectionMatrix);
	}

	//Play Cassette of Sphere
	glBindVertexArray(vao_sphere_rvg);

	//Draw Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
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
	if (bLight_rvg == true)
	{
		lightAngle0_rvg += 0.03f;
		lightAngle1_rvg += 0.03f;
		lightAngle2_rvg += 0.03f;

		if (lightAngle0_rvg >= 360.0f)
		{
			lightAngle0_rvg = lightAngle1_rvg = lightAngle2_rvg = 0.0f;
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

	if (vao_sphere_rvg)
	{
		glDeleteVertexArrays(1, &vao_sphere_rvg);
		vao_sphere_rvg = 0;
	}

	if (vbo_sphere_position_rvg)
	{
		glDeleteBuffers(1, &vbo_sphere_position_rvg);
		vbo_sphere_position_rvg = 0;
	}

	if (vbo_sphere_normal_rvg)
	{
		glDeleteBuffers(1, &vbo_sphere_normal_rvg);
		vbo_sphere_normal_rvg = 0;
	}

	///////////////////////// Safe Shader Cleanup /////////////////////////
	if (gShaderProgramObject_pv_rvg)
	{
		glUseProgram(gShaderProgramObject_pv_rvg);
		
		GLsizei shaderCount;
		GLuint *pShader = NULL;
		glGetProgramiv(gShaderProgramObject_pv_rvg, GL_ATTACHED_SHADERS, &shaderCount);

		pShader = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(gShaderProgramObject_pv_rvg, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_pv_rvg, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		free(pShader);

		glDeleteProgram(gShaderProgramObject_pv_rvg);
		gShaderProgramObject_pv_rvg = 0;
		glUseProgram(0);
	}

}











