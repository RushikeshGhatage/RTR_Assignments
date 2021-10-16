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

GLfloat RAngle_rvg = 0.0f;

bool bLight = false;

GLfloat lightAmbientOne[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuseOne[] = {1.0f, 0.0f, 0.0f, 1.0f};	//Red
GLfloat lightSpecularOne[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightPositionOne[] = {100.0f, 100.0f, 100.0f, 1.0f};	//Directional Light

GLfloat lightAmbientTwo[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuseTwo[] = {0.0f, 0.0f, 1.0f, 1.0f};	//Blue
GLfloat lightSpecularTwo[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightPositionTwo[] = {-100.0f, 100.0f, 100.0f, 1.0f};	//Directional Light

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

GLUquadric* quadric = NULL;

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
							
						case 'L':
						case 'l':
							if (bLight == false)
							{
								bLight = true; 
								glEnable(GL_LIGHTING);
							}
							else
							{
								bLight = false;
								glDisable(GL_LIGHTING);
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
	
	XStoreName(gpDisplay, gWindow, "Rushikesh Vinod Ghatage : Two Lights On Pyramid");
	
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
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularOne);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPositionOne);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecularTwo);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPositionTwo);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
		
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

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

	glTranslatef(0.0f, 0.0f, -4.5f);

	glRotatef(RAngle_rvg, 0.0f, 1.0f, 0.0f);
	////////////////////////////// PYRAMID //////////////////////////////
	glNormal3f(0.0f, 0.447214f, 0.894427f);
	glBegin(GL_TRIANGLES);		//Front Plane
		glVertex3f(0.0f, 1.0f, 0.0f);		//O
		glVertex3f(-1.0f, -1.0f, 1.0f);		//A
		glVertex3f(1.0f, -1.0f, 1.0f);		//B
	glEnd();

	glNormal3f(0.0f, 0.447214f, -0.894427f);
	glBegin(GL_TRIANGLES);		//Back Plane
		glVertex3f(0.0f, 1.0f, 0.0f);		//O
		glVertex3f(1.0f, -1.0f, -1.0f);		//D
		glVertex3f(-1.0f, -1.0f, -1.0f);	//C
	glEnd();

	glNormal3f(-0.894427f, 0.447214f, 0.0f);
	glBegin(GL_TRIANGLES);		//Left Plane
		glVertex3f(0.0f, 1.0f, 0.0f);		//O
		glVertex3f(-1.0f, -1.0f, -1.0f);	//A
		glVertex3f(-1.0f, -1.0f, 1.0f);		//D
	glEnd();

	glNormal3f(0.894427f, 0.447214f, 0.0f);
	glBegin(GL_TRIANGLES);		//Right Plane
		glVertex3f(0.0f, 1.0f, 0.0f);		//O
		glVertex3f(1.0f, -1.0f, 1.0f);		//B
		glVertex3f(1.0f, -1.0f, -1.0f);		//C
	glEnd();
	RAngle_rvg += 0.5f;

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update()
{
	//Code
	if (RAngle_rvg >= 360.0f)
	{
		RAngle_rvg = 0.0f;
	}
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



















































