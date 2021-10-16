//Header
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include <GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include "vmath.h"

//namespace
using namespace std;
using namespace vmath;


enum 
{
	RVG_ATTRIBUTE_POSITION = 0,
	RVG_ATTRIBUTE_COLOR,
	RVG_ATTRIBUTE_NORMAL,
	RVG_ATTRIBUTE_TEXCOORD,
};

//Global Variable Declaration
Display 		*gpDisplay_rvg = NULL;
XVisualInfo 	*gpXVisualInfo_rvg = NULL;
FILE 			*gpFile_rvg = NULL;
bool 			bFullscreen_rvg = false;
Colormap 		gColormap_rvg;
GLXContext 		gGLXContext_rvg;
GLXFBConfig 	gGLXFBConfig_rvg;
Window 			gWindow_rvg;

int giWindowWidth_rvg = 800;
int giWindowHeight_rvg = 600;

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

GLuint gVertexShaderObject_rvg;
GLuint gFragmentShaderObject_rvg;
GLuint gShaderProgramObject_rvg;

GLuint vao_rvg;					//Vertex Array Objects
GLuint vbo_position_rvg;		//Vertex Buffer Objects
GLuint mvpMatrixUniform_rvg;

mat4 orthographicProjectionMatrix_rvg;

//Entry-Point Function
int main(void)
{
	//Local Variable Declaration
	int winWidth_rvg = giWindowWidth_rvg;
	int winHeight_rvg = giWindowHeight_rvg;
	bool bDone_rvg = false;

	//Function Prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);

	void Initialize(void);
	void Resize(int, int);
	void Draw(void);
	void Update(void);
	void Uninitialize();
	
	//Code
	gpFile_rvg = fopen("LogFile.txt", "w");
	if (gpFile_rvg == NULL)
	{
		printf("Log File Cannot open.\n");
		exit(0);
	}

	CreateWindow();
	Initialize();
	ToggleFullscreen();
	
	//Game Loop
	XEvent event;
	KeySym keysym;
	
	while (bDone_rvg == false)
	{
		while (XPending(gpDisplay_rvg))
		{
			XNextEvent(gpDisplay_rvg, &event);
			switch(event.type)
			{
				case MapNotify:
					break;
				
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay_rvg, event.xkey.keycode,0,0);
					switch(keysym)
					{
						case XK_Escape:
							bDone_rvg = true;
							
						case XK_F:
						case XK_f:
							if(bFullscreen_rvg ==  false)
							{
								ToggleFullscreen();
								bFullscreen_rvg = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen_rvg = false;
							}
							break;
							
						default:
							break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							break;
							
						case 2:
							break;
							
						case 3:
							break;
							
						default:
							break;
					}
					break;
					
				case MotionNotify:
					break;
					
				case ConfigureNotify:
					winWidth_rvg = event.xconfigure.width;
					winHeight_rvg = event.xconfigure.height;
					Resize(winWidth_rvg, winHeight_rvg);
					break;
					
				case Expose:
					break;
					
				case DestroyNotify:
					break;
					
				case 33:
					bDone_rvg = true;
					
				default:
					break;			
			}
		}
		//Here You Should Call Update Function for OpenGL Rendering
		Update();

		//Here You Should Call Display Function for OpenGL Rendering
		Draw();
	}
	
	Uninitialize();
	return(0);
}

void CreateWindow(void)
{
	//Local Variable Declaration
	XSetWindowAttributes 	winAttribs_rvg;
	GLXFBConfig 			*pGLXFBConfig_rvg = NULL;
	GLXFBConfig 			bestGLXFBConfig_rvg;
	XVisualInfo 			*pTempXVisualInfo_rvg = NULL;

	int bestFrameBufferConfig_rvg = -1;
	int worstFrameBufferConfig_rvg = -1;
	int bestNumberOfSamples_rvg = -1;
	int worstNumberOfSamples_rvg = 999; 

	int defaultScreen_rvg;
	int styleMask_rvg;
	int numFBConfig_rvg = 0;

	//Function Prototype
	void Uninitialize();

	//Code
	static int frameBufferAttribute_rvg[] = {GLX_X_RENDERABLE, True,
										GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
										GLX_RENDER_TYPE, GLX_RGBA_BIT,
										GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
										GLX_RED_SIZE, 8,
										GLX_GREEN_SIZE, 8,
										GLX_BLUE_SIZE, 8,
										GLX_ALPHA_SIZE, 8,
										GLX_STENCIL_SIZE, 8,
										GLX_DEPTH_SIZE, 24,	//V4L recomands depth size 24
										GLX_DOUBLEBUFFER, True,
										None};
	
	gpDisplay_rvg = XOpenDisplay(NULL);
	if(gpDisplay_rvg == NULL)
	{
		printf("ERROR : Unable to Open X Display. \nExitting Now...");
		Uninitialize();
		exit(1);
	}
	
	defaultScreen_rvg = XDefaultScreen(gpDisplay_rvg);

	pGLXFBConfig_rvg = glXChooseFBConfig(gpDisplay_rvg, defaultScreen_rvg, frameBufferAttribute_rvg, &numFBConfig_rvg);
	if(pGLXFBConfig_rvg == NULL)
	{
		fprintf(gpFile_rvg, "ERROR : Unable to get Framebuffer config. \n Exitting now...");
		Uninitialize();
		exit(1);
	}
		
	for (int i = 0; i < numFBConfig_rvg; i++)
	{
		pTempXVisualInfo_rvg = glXGetVisualFromFBConfig(gpDisplay_rvg, pGLXFBConfig_rvg[i]);
		if (pTempXVisualInfo_rvg)
		{
			int sampleBuffer, sample;

			glXGetFBConfigAttrib(gpDisplay_rvg, pGLXFBConfig_rvg[i], GLX_SAMPLE_BUFFERS, &sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay_rvg, pGLXFBConfig_rvg[i], GLX_SAMPLES, &sample);

			if (bestFrameBufferConfig_rvg < 0 || sampleBuffer && sample > bestNumberOfSamples_rvg)
			{
				bestFrameBufferConfig_rvg = i;
				bestNumberOfSamples_rvg = sample;
			}

			if (worstFrameBufferConfig_rvg < 0 || !sampleBuffer || sample < worstNumberOfSamples_rvg)
			{
				worstFrameBufferConfig_rvg = i;
				worstNumberOfSamples_rvg = sample;
			}
		}

		XFree(pTempXVisualInfo_rvg);
		pTempXVisualInfo_rvg = NULL;
	}

	bestGLXFBConfig_rvg = pGLXFBConfig_rvg[bestFrameBufferConfig_rvg];
	gGLXFBConfig_rvg = bestGLXFBConfig_rvg;

	XFree(pGLXFBConfig_rvg);

	gpXVisualInfo_rvg = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo_rvg == NULL)
	{
		printf("ERROR : Unable to Allocate Memory for Visual Info. \n Exitting now...");
		Uninitialize();
		exit(1);
	}

	gpXVisualInfo_rvg = glXGetVisualFromFBConfig(gpDisplay_rvg, bestGLXFBConfig_rvg);
	
	winAttribs_rvg.border_pixel = 0;
	winAttribs_rvg.background_pixmap = 0;
	
	winAttribs_rvg.colormap = XCreateColormap(gpDisplay_rvg,
										RootWindow(gpDisplay_rvg, gpXVisualInfo_rvg->screen),
										gpXVisualInfo_rvg->visual,
										AllocNone);

	gColormap_rvg = winAttribs_rvg.colormap;
	
	winAttribs_rvg.background_pixel = BlackPixel(gpDisplay_rvg, defaultScreen_rvg);
	
	winAttribs_rvg.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask_rvg = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow_rvg = XCreateWindow(gpDisplay_rvg,
					RootWindow(gpDisplay_rvg, gpXVisualInfo_rvg->screen),
					0,
					0,
					giWindowWidth_rvg,
					giWindowHeight_rvg,
					0,
					gpXVisualInfo_rvg->depth,
					InputOutput,
					gpXVisualInfo_rvg->visual,
					styleMask_rvg,
					&winAttribs_rvg);
					
	if(!gWindow_rvg)
	{
		printf("ERROR : Failed to Create Main Window.\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay_rvg, gWindow_rvg, "Rushikesh Vinod Ghatage : Orthographic Projection");
	
	Atom windowManagerDelete = XInternAtom(gpDisplay_rvg, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay_rvg, gWindow_rvg, &windowManagerDelete, 1);
	
	XMapWindow(gpDisplay_rvg, gWindow_rvg);
	
}

void ToggleFullscreen(void)
{
	//Local Variable Declaration
	Atom wm_state_rvg;
	Atom fullscreen_rvg;
	XEvent xev_rvg = {0};
	
	//Code
	wm_state_rvg = XInternAtom(gpDisplay_rvg, "_NET_WM_STATE", False);
	memset(&xev_rvg, 0, sizeof(xev_rvg));
	
	xev_rvg.type = ClientMessage;
	xev_rvg.xclient.window = gWindow_rvg;
	xev_rvg.xclient.message_type = wm_state_rvg;
	xev_rvg.xclient.format = 32;
	xev_rvg.xclient.data.l[0] = bFullscreen_rvg ? 0 : 1;
	
	fullscreen_rvg = XInternAtom(gpDisplay_rvg, "_NET_WM_STATE_FULLSCREEN", False);
	xev_rvg.xclient.data.l[1] = fullscreen_rvg;
	
	XSendEvent(gpDisplay_rvg,
			RootWindow(gpDisplay_rvg, gpXVisualInfo_rvg->screen),
			False,
			StructureNotifyMask,
			&xev_rvg);
}

void Initialize(void)
{
	//Local Variable Declaration
	Bool isDirectContext_rvg;
	GLint infoLogLength_rvg;
	GLint shaderCompilationStatus_rvg;
	GLchar *szBuffer_rvg;

	//Function Prototype
	void Resize(int, int);
	void Uninitialize(void);

	//Code
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	const int AttribsArray_rvg[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
								GLX_CONTEXT_MINOR_VERSION_ARB, 6,
								GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
								None}; 

	gGLXContext_rvg = glXCreateContextAttribsARB(gpDisplay_rvg, gGLXFBConfig_rvg, 0, True, AttribsArray_rvg);
	if (!gGLXContext_rvg)
	{
		const int AttribsArray_rvg[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
								GLX_CONTEXT_MINOR_VERSION_ARB, 0,
								0}; 

		gGLXContext_rvg = glXCreateContextAttribsARB(gpDisplay_rvg, gGLXFBConfig_rvg, 0, True, AttribsArray_rvg);
	}

	glXMakeCurrent(gpDisplay_rvg, gWindow_rvg, gGLXContext_rvg);

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		fprintf(gpFile_rvg, "glewInit() Failed\n");
		Uninitialize();
		exit(1);
	}

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition;" \
									"uniform mat4 u_mvpMatrix;" \
									"void main(void)" \
									"{"	\
										"gl_Position = u_mvpMatrix * vPosition;" \
									"}";

	//Feed to Shader Object
	glShaderSource(gVertexShaderObject_rvg, 1, (const GLchar **) &VertexShaderCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gVertexShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gVertexShaderObject_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "Vertex Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
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
									"out vec4 fragColor;" \
									"void main(void)" \
									"{"	\
										"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
									"}";

	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_rvg, 1, (const GLchar **) &FragmentShaderCode, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gFragmentShaderObject_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gFragmentShaderObject_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "Fragment Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
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
	glAttachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);	//Fragment Shader

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_POSITION, "vPosition");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_rvg);	

	//Shader Linkage Error Checking
	infoLogLength_rvg = 0;
	GLint shaderProgramLinkStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetProgramiv(gShaderProgramObject_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus_rvg);
	if (shaderProgramLinkStatus_rvg == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetProgramInfoLog(gShaderProgramObject_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "Shader Program Link Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "Shader Program Object Compilation is done Successfully.\n");
	}

	//Get MVP Uniform Location
	mvpMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_mvpMatrix");

	//Vertices Array Declaration
	const GLfloat triangleVertices_rvg[] = 
	{
		0.0f, 50.0f, 0.0f, 		//Top
		-50.0f, -50.0f, 0.0f, 	//Left Bottom
		50.0f, -50.0f, 0.0f		//Right Bottom
	};

	//Create Cassette to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_rvg);
	glBindVertexArray(vao_rvg);

			//Generate Buffer Array
			glGenBuffers(1, &vbo_position_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rvg);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices_rvg), triangleVertices_rvg, GL_STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			//Enable RVG_ATTRIBUTE_POSITION Point
			glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Stop Creating Cassette
	glBindVertexArray(0);	


	//SetClearColor
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	//SetClearDepth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	orthographicProjectionMatrix_rvg = mat4::identity();

	Resize(giWindowWidth_rvg, giWindowHeight_rvg);
}

void Resize(int width, int height)
{
	//Code
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	if (width <= height)
	{
		orthographicProjectionMatrix_rvg = vmath::ortho(-100.0f,
													100.0f,
													-100.0f * ((GLfloat)height / (GLfloat)width),
													100.0f * ((GLfloat)height / (GLfloat)width),
													-100.0f,
													100.0f);
	}
	else
	{
		orthographicProjectionMatrix_rvg = vmath::ortho(-100.0f * ((GLfloat)width / (GLfloat)height),
													100.0f * ((GLfloat)width / (GLfloat)height),
													-100.0f,
													100.0f,
													-100.0f,
													100.0f);
	}

}

void Draw(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	glUseProgram(gShaderProgramObject_rvg);

	//OpenGL Drawing Code
	mat4 modelViewProjectionMatrix;
	mat4 projectionMatrix;

	//Set modelViewMatrix and projectionMatrix to Identity
	modelViewProjectionMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	modelViewProjectionMatrix = orthographicProjectionMatrix_rvg * modelViewProjectionMatrix;	//ORDER IS IMPORTANT

	glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

	//Play Cassette
	glBindVertexArray(vao_rvg);

	//Draw Object
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Stop use Shader Program Object
	glUseProgram(0);

	glXSwapBuffers(gpDisplay_rvg, gWindow_rvg);
}

void Update()
{
	//Code

}

void Uninitialize(void)
{
	//Local Variable Declaration
	GLXContext currentGlxContext_rvg;
	GLsizei shaderCount_rvg;
	GLuint *pShader_rvg = NULL;

	//Code
	currentGlxContext_rvg = glXGetCurrentContext();
	if (currentGlxContext_rvg == gGLXContext_rvg)
	{
		glXMakeCurrent(gpDisplay_rvg, 0, 0);
	}

	if (gGLXContext_rvg)
	{
		glXDestroyContext(gpDisplay_rvg, gGLXContext_rvg);
	}

	if(gWindow_rvg)
	{
		XDestroyWindow(gpDisplay_rvg, gWindow_rvg);
	}
	
	if(gColormap_rvg)
	{
		XFreeColormap(gpDisplay_rvg, gColormap_rvg);
	}
	
	if(gpXVisualInfo_rvg)
	{
		free(gpXVisualInfo_rvg);
		gpXVisualInfo_rvg = NULL;
	}

	if (gpFile_rvg)
	{
		fclose(gpFile_rvg);
		gpFile_rvg = NULL;
	}

	if(gpDisplay_rvg)
	{
		XCloseDisplay(gpDisplay_rvg);
		gpDisplay_rvg = NULL;
	}

	///////////////////////// Safe Shader Cleanup /////////////////////////
	if (gShaderProgramObject_rvg)
	{
		glUseProgram(gShaderProgramObject_rvg);
		
		glGetProgramiv(gShaderProgramObject_rvg, GL_ATTACHED_SHADERS, &shaderCount_rvg);

		pShader_rvg = (GLuint *)malloc(sizeof(GLuint) * shaderCount_rvg);

		glGetAttachedShaders(gShaderProgramObject_rvg, shaderCount_rvg, &shaderCount_rvg, pShader_rvg);

		for (GLsizei i = 0; i < shaderCount_rvg; i++)
		{
			glDetachShader(gShaderProgramObject_rvg, pShader_rvg[i]);
			glDeleteShader(pShader_rvg[i]);
			pShader_rvg[i] = 0;
		}
		free(pShader_rvg);

		glDeleteProgram(gShaderProgramObject_rvg);
		gShaderProgramObject_rvg = 0;
		glUseProgram(0);
	}

}



