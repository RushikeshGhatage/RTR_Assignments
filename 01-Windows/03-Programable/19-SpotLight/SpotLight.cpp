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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

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

GLuint LaUniform, LdUniform, LsUniform;
GLuint KaUniform, KdUniform, KsUniform;
GLint shininessUniform;
GLuint lightPositionUniform;

//SpotGLight Uniform
GLuint spotlightDirectionUniform;
GLuint spotlightPositionUniform;
GLuint spotLightCutoffUniform;
GLuint spotlightExponentUniform;

GLint constAttUniform;
GLint linearAttUniform;
GLint quadAttUniform;

GLfloat spotlightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};
GLfloat spotlightDirection[] = {0.0f, 0.0f, -1.0f};
GLfloat spotlightCutoff = 0.0f;
GLfloat spotlightExponent;
GLfloat constAttinuation;
GLfloat linearAttinuation;
GLfloat quadAttinuation;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint LKeyPressedUniform;

bool bLight;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};	//Grey Light
GLfloat lightDiffuse[] = {1.0f, 0.560f, 0.0f, 1.0f};	//White Light 
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};


GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;

GLfloat CAngle_rvg = 0.0f;

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
			if (bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
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

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition;" \
									"in vec3 vNormal;" \

									"out vec3 transformed_normal;" \
									"out vec3 view_vector;" \
									"out vec3 light_direction;" \
									
									"uniform vec4 u_spotlight_position;" \
									"uniform int u_lKeyPressed;" \

									"uniform mat4 u_modelMatrix;" \
									"uniform mat4 u_viewMatrix;" \
									"uniform mat4 u_projectionMatrix;" \

									"void main(void)" \
									"{"	\
										"if(u_lKeyPressed == 1)" \
										"{" \
											"vec4 eye_coordinates = (u_viewMatrix * u_modelMatrix * vPosition);" \
											"transformed_normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
											"light_direction = vec3(u_spotlight_position - eye_coordinates);" \
											"view_vector = (-eye_coordinates.xyz);" \
										"}" \
										"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
									"}";		

	//Feed to Shader Object
	glShaderSource(gVertexShaderObject, 1, (const GLchar **) &VertexShaderCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject);	

	//Shader Compilation Error Checking
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	GLchar *szBuffer = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, infoLogLength, &written, szBuffer);
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
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode = 
									"#version 460 core" \
									"\n" \
									"out vec4 fragColor;" \

									"in vec3 transformed_normal;" \
									"in vec3 light_direction;" \
									"in vec3 view_vector;" \

									"uniform vec3 u_la;" \
									"uniform vec3 u_ld;" \
									"uniform vec3 u_ls;" \

									"uniform vec3 u_ka;" \
									"uniform vec3 u_kd;" \
									"uniform vec3 u_ks;" \

									"uniform vec3 u_spotlight_direction;" \
									"uniform float u_spotCutoff;" \
									"uniform float u_spotExponent;" \
									"uniform float u_constantAtt;" \
									"uniform float u_linearAtt;" \
									"uniform float u_quadAtt;" \

									"uniform float u_materialShininess;" \
									"out vec3 fong_ads_light;" \

									"uniform int u_lKeyPressed;" \

									"void main(void)" \
									"{"	\
									"fong_ads_light = vec3(0.0, 0.0, 0.0);" \

									"if(u_lKeyPressed == 1)" \
										"{" \
											"vec3 normalized_tranformed_normal = normalize(transformed_normal);" \
											"vec3 normalized_light_direction = normalize(light_direction);" \
											"vec3 normalized_view_vector = normalize(view_vector);"
											"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" \
											"float distance = length(light_direction);" \
											"float attinuation = (1.0 / (u_constantAtt + u_linearAtt * distance + u_quadAtt * distance * distance));" \
											"float spotAngle = max(dot(-normalized_light_direction, normalize(u_spotlight_direction)), 0.0);" \
											"float spotAttinuation;" \
											"if(spotAngle > u_spotCutoff)" \
												"{" \
													"spotAttinuation = max(pow(spotAngle, u_spotExponent), 0.0);" \
												"}" \
											"else" \
											"{" \
												"spotAttinuation = 0.0;" \
											"}" \
											"attinuation = attinuation * spotAttinuation;"
											"vec3 ambient = u_la * u_ka;" \
											"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" \
											"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess);" \
											"fong_ads_light = spotAttinuation * (ambient + diffuse + specular);" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"fragColor = vec4(fong_ads_light, 1.0);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **) &FragmentShaderCode, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, infoLogLength, &written, szBuffer);
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
	gShaderProgramObject = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject, gVertexShaderObject);		//Vertex Shader
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);	//Gragment Shader

	//3)(Post-Attach)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject, RVG_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, RVG_ATTRIBUTE_NORMAL, "vNormal");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject);	

	//Shader Linkage Error Checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, infoLogLength, &written, szBuffer);
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
	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");

	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_materialShininess");

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");

	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

	spotlightDirectionUniform = glGetUniformLocation(gShaderProgramObject, "u_spotlight_direction");
	spotlightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_spotlight_position");
	spotLightCutoffUniform = glGetUniformLocation(gShaderProgramObject, "u_spotCutoff");
	spotlightExponentUniform = glGetUniformLocation(gShaderProgramObject, "u_spotExponent");
	constAttUniform = glGetUniformLocation(gShaderProgramObject, "u_constantAtt");
	linearAttUniform = glGetUniformLocation(gShaderProgramObject, "u_linearAtt");
	quadAttUniform = glGetUniformLocation(gShaderProgramObject, "u_quadAtt");

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
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(LsUniform, 1, lightSpecular);

		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffuse);
		glUniform3fv(KsUniform, 1, materialSpecular);

		glUniform1f(shininessUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, spotlightPosition);
		
		glUniform3fv(spotlightDirectionUniform, 1, spotlightDirection);
		glUniform1f(spotlightExponentUniform, 100.0f);
		glUniform1f(spotLightCutoffUniform, cos(15 * 3.14 / 180.0f));

		glUniform1f(constAttUniform, 1.0f);
		glUniform1f(linearAttUniform, 0.5f);
		glUniform1f(quadAttUniform, 1.0f);

	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
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

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

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
	CAngle_rvg += 1.0f;

	if (CAngle_rvg >= 360.0f)
	{
		CAngle_rvg = 0.0f;
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
	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		
		GLsizei shaderCount;
		GLuint *pShader = NULL;
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		pShader = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		free(pShader);

		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}

}











