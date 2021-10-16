#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<GL/gl.h>	//For OpenGL API's
#include<GL/glx.h>
#include<GL/glu.h>


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//namespace
using namespace std;

//global variable declaration
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGlxContext;

//entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);
	
	void Initialize(void);
	void Resize(int, int);
	void Draw(void);
	void Update(void);
	void Uninitialize();
	
	//variable declaration
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	bool bDone = false;
	
	//code
	CreateWindow();
	
	Initialize();
	ToggleFullscreen();
	
	//Game Loop
	XEvent event;
	KeySym keysym;
	
	while (bDone == false)
	{
		while (XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
			switch(event.type)
			{
				case MapNotify:
					break;
				
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode,0,0);
					switch(keysym)
					{
						case XK_Escape:
							bDone = true;
							
						case XK_F:
						case XK_f:
							if(bFullscreen ==  false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
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
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					Resize(winWidth, winHeight);
					break;
					
				case Expose:
					break;
					
				case DestroyNotify:
					break;
					
				case 33:
					bDone = true;
					
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
	//function prototype
	void Uninitialize();
	
	//variable declaration
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int styleMask;

	static int frameBufferAttribute[] = {GLX_DOUBLEBUFFER, True,
										GLX_RGBA,
										GLX_RED_SIZE, 8,
										GLX_GREEN_SIZE, 8,
										GLX_BLUE_SIZE, 8,
										GLX_ALPHA_SIZE, 8,
										GLX_DEPTH_SIZE, 24,	//V4L recomands depth size 24
										None};
	
	//code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable to Open X Display. \nExitting Now...");
		Uninitialize();
		exit(1);
	}
	
	defaultScreen = XDefaultScreen(gpDisplay);
		
	gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable to Allocate Memory for Visual Info. \n Exitting now...");
		Uninitialize();
		exit(1);
	}

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttribute);
	
	//XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
	if(gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable to get A Visual Info. \n Exitting now...");
		Uninitialize();
		exit(1);
	}
	
	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	
	winAttribs.colormap = XCreateColormap(gpDisplay,
										RootWindow(gpDisplay, gpXVisualInfo->screen),
										gpXVisualInfo->visual,
										AllocNone);

	gColormap = winAttribs.colormap;
	
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow(gpDisplay,
					RootWindow(gpDisplay, gpXVisualInfo->screen),
					0,
					0,
					giWindowWidth,
					giWindowHeight,
					0,
					gpXVisualInfo->depth,
					InputOutput,
					gpXVisualInfo->visual,
					styleMask,
					&winAttribs);
					
	if(!gWindow)
	{
		printf("ERROR : Failed to Create Main Window.\n Exitting now...");
		Uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "Rushikesh Vinod Ghatage : Static India");
	
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	
	XMapWindow(gpDisplay, gWindow);
	
}

void ToggleFullscreen(void)
{
	//variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};
	
	//code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(xev));
	
	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;
	
	XSendEvent(gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			False,
			StructureNotifyMask,
			&xev);
}

void Initialize(void)
{
	//Function Prototype
	void Resize(int, int);

	//Code
	gGlxContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGlxContext);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//SetClearDepth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	Resize(giWindowWidth, giWindowHeight);
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

void Draw(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -7.5f);

		///////////////////////////////////////// I /////////////////////////////////////////
	

	////////////////////////// Upper I //////////////////////////

	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.00f, 0.50f, 0.00f);		//A
	
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.80f, 0.50f, 0.00f);		//B
	
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.80f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.80f, 0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F
	
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.70f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.80f, 0.30f, 0.00f);		//D

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.00f, 0.30f, 0.00f);		//C
	glEnd();
	
	
	glBegin(GL_TRIANGLES);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-2.00f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.10f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-2.00f, 0.30f, 0.00f);		//C
	glEnd();


	////////////////////////// Middle I //////////////////////////
	

	glBegin(GL_TRIANGLES);		//Top
	
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.30f, 0.30f, 0.00f);		//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-2.50f, 0.30f, 0.00f);		//B

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.50f, 0.30f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.50f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.50f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-2.30f, -0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.30f, 0.30f, 0.00f);		//A

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.40f, 0.20f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.40f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-2.30f, -0.30f, 0.00f);		//D
	glEnd();
	
	
	////////////////////////// Lower I //////////////////////////
	
	
	glBegin(GL_POLYGON);		//Top
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.00f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.80f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.80f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.80f, -0.50f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.70f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.80f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.00f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-2.00f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.10f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-2.00f, -0.50f, 0.00f);		//D
	glEnd();


	///////////////////////////////////////// N /////////////////////////////////////////
	

	////////////////////////// Left N //////////////////////////


	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.50f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.70f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-1.70f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-1.70f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.70f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-1.50f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-1.50f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-1.60f, 0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-1.60f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-1.50f, -0.50f, 0.00f);		//D
	glEnd();
	

	////////////////////////// Middle N //////////////////////////


	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.90f, -0.20f, 0.00f);		//A

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-1.50f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-1.50f, 0.35f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.90f, -0.35f, 0.00f);		//F
	glEnd();

	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.90f, -0.35f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-1.50f, 0.35f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(-1.50f, 0.20f, 0.00f);		//C

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.90f, -0.50f, 0.00f);		//D
	glEnd();


	////////////////////////// Right N //////////////////////////


	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.70f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.90f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.90f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.90f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.90f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.70f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-0.70f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.80f, 0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.80f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(-0.70f, -0.50f, 0.00f);		//D
	glEnd();

		
	///////////////////////////////////////// D /////////////////////////////////////////


	glBegin(GL_POLYGON);		//Outer Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.20f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.40f, 0.50f, 0.00f);		//C

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.125f, 0.4f, 0.0f);			//H
	glEnd();

	glBegin(GL_POLYGON);		//Inner Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.125f, 0.4f, 0.0f);			//H

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.05f, 0.30f, 0.00f);		//b
	glEnd();
	
	glBegin(GL_POLYGON);		//Outer Upper Left
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(-0.40f, 0.50f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.40f, 0.00f, 0.00f);		//D

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J
	glEnd();

	
	glBegin(GL_POLYGON);		//Inner Upper Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(-0.307f, 0.4f, 0.0f);		//I

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.214f, 0.00f, 0.00f);		//d
	glEnd();


	glBegin(GL_POLYGON);		//Outer Lower Left
	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(-0.307f, 0.0f, 0.0f);		//J

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.40f, 0.00f, 0.0f);		//D

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.40f, -0.50f, 0.0f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K
	glEnd();


	glBegin(GL_POLYGON);		//Inner Lower Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.214f, 0.00f, 0.0f);		//d

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(-0.307f, 0.00f, 0.0f);		//J

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.214f, -0.30f, 0.0f);		//e
	glEnd();


	glBegin(GL_POLYGON);		//Outer Bottom
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(-0.40f, -0.50f, 0.0f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.20f, -0.50f, 0.0f);		//F
	glEnd();


	glBegin(GL_POLYGON);		//Inner Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.05f, -0.30f, 0.0f);		//f

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.214f, -0.30f, 0.0f);		//e

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(-0.307f, -0.40f, 0.0f);		//K

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L
	glEnd();
	
	
	glBegin(GL_POLYGON);		//Outer Lower Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.50f, 0.00f, 0.0f);			//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.20f, -0.50f, 0.0f);		//F
	glEnd();


	glBegin(GL_POLYGON);		//Inner Lower Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.25f, 0.00f, 0.0f);			//a
	
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.05f, -0.30f, 0.0f);		//f

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.125f, -0.40f, 0.0f);		//L
	glEnd();

	
	glBegin(GL_POLYGON);		//Outer Upper Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.20f, 0.50f, 0.0f);			//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.125f, 0.40f, 0.0f);		//H

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.50f, 0.00f, 0.0f);			//A
	glEnd();


	glBegin(GL_POLYGON);		//Inner Upper Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.125f, 0.40f, 0.0f);		//H

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.005f, 0.30f, 0.0f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.25f, 0.00f, 0.00f);		//a

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.375f, 0.00f, 0.0f);		//G
	glEnd();

	
	glBegin(GL_POLYGON);		//Inner
	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.25f, 0.00f, 0.00f);		//a

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.05f, 0.30f, 0.00f);		//b
	
	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(-0.214f, 0.30f, 0.00f);		//c

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(-0.214f, -0.30f, 0.00f);		//d

	glColor3f(0.0f, 0.0f, 0.0f);		//Original Black
	glVertex3f(0.05f, -0.30f, 0.00f);		//e
	glEnd();
	

	///////////////////////////////////////// I /////////////////////////////////////////


	////////////////////////// Upper I //////////////////////////


	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.45f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.65f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.65f, 0.50f, 0.00f);		//B

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(0.65f, 0.30f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.75f, 0.40f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(0.65f, 0.30f, 0.00f);		//C

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.45f, 0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(1.45f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.35f, 0.40f, 0.00f);		//F

	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(1.45f, 0.30f, 0.00f);		//D
	glEnd();


	////////////////////////// Middle I //////////////////////////
	
	
	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.15f, 0.30f, 0.00f);		//A

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(0.95f, 0.30f, 0.00f);		//B

	glColor3f(0.752f, 0.752f, 0.752f);		//Light White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.95f, 0.30f, 0.00f);		//B

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(0.95f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(0.95f, -0.30f, 0.00f);		//C

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(1.15f, -0.30f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.15f, 0.30f, 0.00f);		//A

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.05f, 0.20f, 0.00f);		//E

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.05f, -0.20f, 0.00f);		//F

	glColor3f(0.627f, 0.627f, 0.627f);		//Dark White
	glVertex3f(1.15f, -0.30f, 0.00f);		//D
	glEnd();

	
	////////////////////////// Lower I //////////////////////////
	
	
	glBegin(GL_POLYGON);		//Top
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.45f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.65f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F
	glEnd();


	glBegin(GL_TRIANGLES);		//Left
	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.65f, -0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(0.65f, -0.50f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.75f, -0.40f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(0.65f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.45f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_TRIANGLES);		//Right
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.45f, -0.30f, 0.00f);		//A

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.35f, -0.40f, 0.00f);		//F

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.45f, -0.50f, 0.00f);		//D
	glEnd();


	///////////////////////////////////////// A /////////////////////////////////////////
	

	////////////////////////// Left A //////////////////////////

	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(2.15f, 0.50f, 0.00f);		//F

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.05f, 0.50f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.65f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E
	glEnd();

	
	glBegin(GL_TRIANGLES);		//Bottom
	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E

	glVertex3f(1.65f, -0.50f, 0.00f);		//C

	glVertex3f(1.85f, -0.50f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.25f, 0.50f, 0.00f);		//A

	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.15f, 0.50f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.80f, -0.35f, 0.00f);		//E

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(1.85f, -0.50f, 0.00f);		//D
	glEnd();

	
	////////////////////////// Middle A //////////////////////////

	glBegin(GL_POLYGON);		//Top
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.29f, 0.00f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.08f, 0.00f, 0.00f);		//B

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.27f, -0.055f, 0.00f);		//a
	glEnd();


	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.08f, 0.00f, 0.00f);		//B

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.00f, -0.20f, 0.00f);		//C
	glEnd();


	glBegin(GL_POLYGON);		//Bottom
	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.305f, -0.146f, 0.00f);		//d

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.00f, -0.20f, 0.00f);		//C

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.37f, -0.20f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.29f, 0.00f, 0.00f);		//A

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.27f, -0.055f, 0.00f);		//a

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.305f, -0.146f, 0.00f);		//d

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.37f, -0.20f, 0.00f);		//D
	glEnd();


	glBegin(GL_POLYGON);		//Inner
	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.27f, -0.055f, 0.00f);		//a

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.10f, -0.055f, 0.00f);		//b

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.065f, -0.146f, 0.00f);		//c

	glColor3f(1.0f, 1.0f, 1.0f);			//Original White
	glVertex3f(2.305f, -0.146f, 0.00f);		//d
	glEnd();


	////////////////////////// Right A //////////////////////////
	

	glBegin(GL_POLYGON);		//Left
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.21f, 0.30f, 0.00f);		//B

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.50f, -0.50f, 0.00f);		//C

	glColor3f(0.074f, 0.666f, 0.031f);		//Light Green
	glVertex3f(2.62f, -0.50f, 0.00f);		//F
	glEnd();
	

	glBegin(GL_TRIANGLES);		//Top
	glColor3f(1.0f, 0.800f, 0.2f);			//Light Saffron
	glVertex3f(2.30f, 0.45f, 0.00f);		//A

	glVertex3f(2.21f, 0.30f, 0.00f);		//B

	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glEnd();


	glBegin(GL_POLYGON);		//Right
	glColor3f(1.0f, 0.600f, 0.2f);			//Original Saffron
	glVertex3f(2.30f, 0.45f, 0.00f);		//A

	glColor3f(1.0f, 0.400f, 0.2f);			//Dark Saffron
	glVertex3f(2.34f, 0.17f, 0.00f);		//E

	glColor3f(0.074f, 0.533f, 0.031f);		//Original Green
	glVertex3f(2.62f, -0.50f, 0.00f);		//F

	glColor3f(0.074f, 0.466f, 0.031f);		//Dark Green
	glVertex3f(2.71f, -0.50f, 0.00f);		//D
	glEnd();



	glXSwapBuffers(gpDisplay, gWindow);
}

void Update()
{
	//Code

}

void Uninitialize(void)
{
	//Local Variable Declaration
	GLXContext currentGlxContext;

	currentGlxContext = glXGetCurrentContext();
	if (currentGlxContext == gGlxContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if (gGlxContext)
	{
		glXDestroyContext(gpDisplay, gGlxContext);
	}

	if(gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}



















































