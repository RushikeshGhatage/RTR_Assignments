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
GLuint gTessellationControlShaderObject_rvg;
GLuint gTessellationEvaluationShaderObject_rvg;

GLuint vao_rvg;					//Vertex Array Objects
GLuint vbo_position_rvg;		//Vertex Buffer Objects
GLuint mvpMatrixUniform_rvg;

GLuint numberOfSegmentsUniform_rvg;
GLuint numberOfStripsUniform_rvg;
GLuint lineColorUniform_rvg;

unsigned int uiNumberOfInt_rvg;

mat4 perspectiveProjectionMatrix;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Prototype
	void Initialize(void);
	void ToggleFullScreen(void);
	void Display(void);

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

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		case VK_UP:
			uiNumberOfInt_rvg++;
			if (uiNumberOfInt_rvg >= 50)
			{
				uiNumberOfInt_rvg = 50;
			}
			break;

		case VK_DOWN:
			uiNumberOfInt_rvg--;
			if (uiNumberOfInt_rvg <= 1)
			{
				uiNumberOfInt_rvg = 1;
			}
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
	gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec2 vPosition;" \
									"void main(void)" \
									"{"	\
										"gl_Position = vec4(vPosition, 0.0, 1.0);" \
									"}";		

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

	// ****************** Tessellation Control Shader ******************
	//Create Shader
	gTessellationControlShaderObject_rvg = glCreateShader(GL_TESS_CONTROL_SHADER);

	//Provide Source Code to Shader
	const GLchar *TessellationControlShaderCode = 
									"#version 460 core" \
									"\n" \
									"layout(vertices = 4)out;" \
									"uniform int numberOfSegments;" \
									"uniform int numberOfStrips;" \
									"void main(void)" \
									"{"	\
										"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
										"gl_TessLevelOuter[0] = float(numberOfStrips);" \
										"gl_TessLevelOuter[1] = float(numberOfSegments);" \
									"}";		

	//Feed to Shader Object
	glShaderSource(gTessellationControlShaderObject_rvg, 1, (const GLchar **) &TessellationControlShaderCode, NULL);

	//Compile Shader
	glCompileShader(gTessellationControlShaderObject_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gTessellationControlShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationControlShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationControlShaderObject_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "Tessellation Control Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Tessellation Control Shader Compilation is done Successfully.\n");
	}

	// ****************** Tessellation Evaluation Shader ******************
	//Create Shader
	gTessellationEvaluationShaderObject_rvg = glCreateShader(GL_TESS_EVALUATION_SHADER);

	//Provide Source Code to Shader
	const GLchar *TessellationEvaluationShaderCode = 
									"#version 460 core" \
									"\n" \
									"layout(isolines)in;" \
									"uniform mat4 u_mvpMatrix;" \
									"void main(void)" \
									"{"	\
										"float tessCoord = gl_TessCoord.x;" \
										"vec3 p0 = gl_in[0].gl_Position.xyz;" \
										"vec3 p1 = gl_in[1].gl_Position.xyz;" \
										"vec3 p2 = gl_in[2].gl_Position.xyz;" \
										"vec3 p3 = gl_in[3].gl_Position.xyz;" \
										"vec3 p = p0 * (1.0 - tessCoord) * (1.0 - tessCoord) * (1.0 - tessCoord) + p1 * 3.0 * tessCoord * (1.0 - tessCoord) * (1.0 - tessCoord) + p2 * 3.0 * tessCoord * tessCoord * (1.0 - tessCoord) + p3 * tessCoord * tessCoord * tessCoord;" \
										"gl_Position = u_mvpMatrix * vec4(p, 1.0);" \
									"}";		

	//Feed to Shader Object
	glShaderSource(gTessellationEvaluationShaderObject_rvg, 1, (const GLchar **) &TessellationEvaluationShaderCode, NULL);

	//Compile Shader
	glCompileShader(gTessellationEvaluationShaderObject_rvg);	

	//Shader Compilation Error Checking
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(gTessellationEvaluationShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationEvaluationShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szBuffer = (GLchar *)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationEvaluationShaderObject_rvg, infoLogLength, &written, szBuffer);
				fprintf(gpFile_rvg, "Tessellation Evaluation Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd_rvg);
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Tessellation Evaluation Shader Compilation is done Successfully.\n");
	}	

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode = 
									"#version 460 core" \
									"\n" \
									"out vec4 fragColor;" \
									"uniform vec4 lineColor;" \
									"void main(void)" \
									"{"	\
										"fragColor = lineColor;" \
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
	glAttachShader(gShaderProgramObject_rvg, gTessellationControlShaderObject_rvg);		//Tessellation Control Shader
	glAttachShader(gShaderProgramObject_rvg, gTessellationEvaluationShaderObject_rvg);		//Tessellation Evaluation Shader
	glAttachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);	//Gragment Shader

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_POSITION, "vPosition");

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

	//Get MVP Uniform Location
	mvpMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_mvpMatrix");
	
	numberOfSegmentsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "numberOfSegments");
	numberOfStripsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "numberOfStrips");
	lineColorUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "lineColor");

	//Vertices Array Declaration
	const GLfloat vertices[] = 
	{
		-1.0f, -1.0f,
		-0.5f, 1.0f,
		0.5f, -1.0f,
		1.0f, 1.0f
	};

	//Create Cassette to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_rvg);
	glBindVertexArray(vao_rvg);

			//Generate Buffer Array
			glGenBuffers(1, &vbo_position_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rvg);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Stop Creating Cassette
	glBindVertexArray(0);

	//SetClearDepth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);		//Hidden Surface Removal

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	uiNumberOfInt_rvg = 1;

	//Set Orthographic Matrix to Identity
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

	//OpenGL Drawing Code
	mat4 modelViewProjectionMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;

	//Set modelViewMatrix and projectionMatrix to Identity
	modelViewProjectionMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

	modelViewProjectionMatrix = translateMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT

	glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1i(numberOfSegmentsUniform_rvg, uiNumberOfInt_rvg);
	glUniform1i(numberOfStripsUniform_rvg, 1);
	glUniform4fv(lineColorUniform_rvg, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	glLineWidth(2.0f);

	//Play Cassette
	glBindVertexArray(vao_rvg);

		//Set Patches
		glPatchParameteri(GL_PATCH_VERTICES, 4);

		//Draw Object
		glDrawArrays(GL_PATCHES, 0, 4);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Stop using Shader Program Object
	glUseProgram(0);
	SwapBuffers(ghdc_rvg);
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

	if (vao_rvg)
	{
		glDeleteVertexArrays(1, &vao_rvg);
		vao_rvg = 0;
	}

	if (vbo_position_rvg)
	{
		glDeleteBuffers(1, &vbo_position_rvg);
		vbo_position_rvg = 0;
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

