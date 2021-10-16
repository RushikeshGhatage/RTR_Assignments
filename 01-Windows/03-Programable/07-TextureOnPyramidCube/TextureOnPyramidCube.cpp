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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint vao_pyramid;					//Vertex Array Objects
GLuint vbo_position_pyramid;		//Vertex Buffer Objects for Vertex
GLuint vbo_texture_pyramid;			//Vertex Buffer Objects for Colors

GLuint vao_cube;					//Vertex Array Objects
GLuint vbo_position_cube;			//Vertex Buffer Objects for Vertex
GLuint vbo_texture_cube;			//Vertex Buffer Objects for Colors

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;

GLfloat PAngle_rvg = 0.0f;
GLfloat CAngle_rvg = 0.0f;

mat4 perspectiveProjectionMatrix;

GLuint Stone_Texture;
GLuint Kundali_Texture;

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
	bool loadGLTexture(GLuint*, TCHAR[]);

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
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition;" \
									"in vec2 vTexcoord;" \
									"uniform mat4 u_mvpMatrix;" \
									"out vec2 out_Texcoord;" \
									"void main(void)" \
									"{"	\
									"gl_Position = u_mvpMatrix * vPosition;" \
									"out_Texcoord = vTexcoord;" \
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
									"in vec2 out_Texcoord;" \
									"uniform sampler2D u_texture_sampler;" \
									"out vec4 fragColor;" \
									"void main(void)" \
									"{"	\
									"fragColor = texture(u_texture_sampler, out_Texcoord);" \
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
				glGetShaderInfoLog(gVertexShaderObject, infoLogLength, &written, szBuffer);
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
	glBindAttribLocation(gShaderProgramObject, RVG_ATTRIBUTE_TEXCOORD, "vTexcoord");

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

	//Get MVP Uniform Location
	mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

	//Vertices Array Declaration
	const GLfloat pyramidVertices[] = 
	{
		0.0f, 0.75f, 0.0f, 		//Apex
		0.5f, -0.25f, 0.5f, 	//Front Left Bottom
		-0.5f, -0.25f, 0.5f,	//Front Right Bottom
		
		0.0f, 0.75f, 0.0f,		//Apex
		0.5f, -0.25f, -0.5f,	//Back Right Bottom
		-0.5f, -0.25f, -0.5f,	//Back Left Bottom

		0.0f, 0.75f, 0.0f,		//Apex
		0.5f, -0.25f, 0.5f,		//Left Left Bottom
		0.5f, -0.25f, -0.5f,	//Left Right Bottom

		0.0f, 0.75f, 0.0f,		//Apex
		-0.5f, -0.25f, 0.5f,	//Right Left Bottom
		-0.5f, -0.25f, -0.5f 	//Right Right Bottom
	};

	const GLfloat pyramidTexcoords[] = 
	{
		0.5f, 1.0f,		//Front
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.5f, 1.0f,		//Back
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.5f, 1.0f,		//Left
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.5f, 1.0f,		//Right
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	const GLfloat cubeVertices[] = 
	{
		-0.5f, 0.5f, 0.5f,		//Top
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		
		-0.5f, -0.5f, 0.5f,		//Bottom
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f,		//Front
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, -0.5f,		//Back
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f, 0.5f,		//Left
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,

		0.5f, 0.5f, 0.5f,		//Right
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f
	};

	const GLfloat cubeTexcoords[] = 
	{
		0.0f, 0.0f,		//Top
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,		//Bottom
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,		//Front
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,		//Back
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,		//Left
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,		//Right
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	//Create Cassette for TRIANGLE to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_position_pyramid);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			/////////////////////////////// FOR TEXTURE ///////////////////////////////
			//Generate Buffer Array
 			glGenBuffers(1, &vbo_texture_pyramid);

			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_pyramid);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoords), pyramidTexcoords, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Stop Creating Cassette for TRIANGLE
	glBindVertexArray(0);

	//Create Cassette for SQUARE to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_position_cube);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			/////////////////////////////// FOR COLOR ///////////////////////////////
			//Generate Buffer Array
 			glGenBuffers(1, &vbo_texture_cube);

			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Stop Creating Cassette for SQUARE
	glBindVertexArray(0);

	//SetClearDepth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);		//Hidden Surface Removal

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Enable and Set Texture
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Stone_Texture, MAKEINTRESOURCE(STONE_BITMAP));
	loadGLTexture(&Kundali_Texture, MAKEINTRESOURCE(KUNDALI_BITMAP));

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

bool loadGLTexture(GLuint* texture, TCHAR resourceID[])
{
	//Local Variable Declaration
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	//Code
	hBitmap = (HBITMAP)(LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		
		//OpenGL Actual Texture Code
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		//Setting Texture Parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Push Data into Graphic Memory with help of Graphic Driver
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return(bResult);
}

void Display(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	glUseProgram(gShaderProgramObject);

	//OpenGL Drawing Code
	mat4 modelViewProjectionMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	//Translation for TRIANGLE
	//Set modelViewMatrix and projectionMatrix to Identity
	modelViewProjectionMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = vmath::translate(-1.25f, -0.2f, -4.0f);
	rotationMatrix = vmath::rotate(PAngle_rvg, 0.0f, 1.0f, 0.0f);

	modelViewProjectionMatrix = translateMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Set Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Stone_Texture);
	glUniform1i(textureSamplerUniform, 0);

	//Play Cassette of Triangle
	glBindVertexArray(vao_pyramid);

	//Draw Object Triangle
	glDrawArrays(GL_TRIANGLES, 0, 12);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Translation for SQUARE
	//Set modelViewMatrix and projectionMatrix to Identity
	modelViewProjectionMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = vmath::translate(1.25f, 0.0f, -4.0f);
	rotationMatrix = vmath::rotate(CAngle_rvg, 1.0f, 0.0f, 0.0f);
	scaleMatrix = vmath::scale(0.85f, 0.85f, 0.85f);

	modelViewProjectionMatrix = translateMatrix * scaleMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewProjectionMatrix;	//ORDER IS IMPORTANT
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Set Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Kundali_Texture);
	glUniform1i(textureSamplerUniform, 0);

	//Play Cassette of SQUARE
	glBindVertexArray(vao_cube);	

	//Draw Object Triangle
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//Stop Playing Cassette for SQUARE
	glBindVertexArray(0);

	//Stop using Shader Program Object
	glUseProgram(0);
	SwapBuffers(ghdc_rvg);
}

void Update()
{
	//Code
	PAngle_rvg += 1.0f;
	CAngle_rvg += 1.0f;

	if (PAngle_rvg >= 360.0f)
	{
		PAngle_rvg = 0.0f;
	}
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

	if (Stone_Texture)
	{
		glDeleteTextures(1, &Stone_Texture);
	}
	
	if (Kundali_Texture)
	{
		glDeleteTextures(1, &Kundali_Texture);
	}

	if (vao_pyramid)
	{
		glDeleteVertexArrays(1, &vao_pyramid);
		vao_pyramid = 0;
	}

	if (vbo_position_pyramid)
	{
		glDeleteBuffers(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}

	if (vbo_texture_pyramid)
	{
		glDeleteBuffers(1, &vbo_texture_pyramid);
		vbo_texture_pyramid = 0;
	}


	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}

	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	if (vbo_texture_cube)
	{
		glDeleteBuffers(1, &vbo_texture_cube);
		vbo_texture_cube = 0;
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











