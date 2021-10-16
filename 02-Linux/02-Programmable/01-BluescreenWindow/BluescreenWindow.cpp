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

//namespace
using namespace std;

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
	fprintf(gpFile_rvg, "Found Number of FBConfig = %d\n",numFBConfig_rvg);
		
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

			fprintf(gpFile_rvg, "Framebuffer Config : %d\n", i);
			fprintf(gpFile_rvg, "Visual ID : %lu\n", pTempXVisualInfo_rvg->visualid);
			fprintf(gpFile_rvg, "Sample Buffer : %d\n", sampleBuffer);
			fprintf(gpFile_rvg, "Sample : %d\n", sample);
			fprintf(gpFile_rvg, "-------------------------------------\n");
		}
		XFree(pTempXVisualInfo_rvg);
		pTempXVisualInfo_rvg = NULL;
	}

	fprintf(gpFile_rvg, "Best Frame Buffer Config : %d\n", bestFrameBufferConfig_rvg);

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

	XStoreName(gpDisplay_rvg, gWindow_rvg, "Rushikesh Vinod Ghatage : BlueScreen");
	
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

	
	isDirectContext_rvg = glXIsDirect(gpDisplay_rvg, gGLXContext_rvg);
	if (isDirectContext_rvg == true)
	{
		fprintf(gpFile_rvg, "Rendering Context is Direct Hardware Rendering Context.\n");
	}
	else
	{
		fprintf(gpFile_rvg, "Rendering Context is Software Rendering Context.\n");
	}

	glXMakeCurrent(gpDisplay_rvg, gWindow_rvg, gGLXContext_rvg);

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		fprintf(gpFile_rvg, "glewInit() Failed\n");
		Uninitialize();
		exit(1);
	}

	//SetClearColor
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	//SetClearDepth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

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

}

void Draw(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
}



