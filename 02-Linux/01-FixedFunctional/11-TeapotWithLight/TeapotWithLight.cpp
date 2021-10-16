#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include "Teapot.h"

#include<GL/gl.h>	//For OpenGL API's
#include<GL/glx.h>
#include<GL/glu.h>

#include<SOIL/SOIL.h>	//For Texture

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

bool gbTexture = false;
GLfloat Angle_rvg = 0.0f;
GLuint Texture_Marble;

GLXContext gGlxContext;

bool gbLight = false;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	//White Light
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f}; //Positional Light

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	//White Object
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess = 128.0f;

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

						case 'T':
						case 't':
							if (gbTexture == false)
							{
								glEnable(GL_TEXTURE_2D);
								gbTexture = true;
							}
							else
							{
								glDisable(GL_TEXTURE_2D);
								gbTexture = false;
							}
							break;

						case 'L':
						case 'l':
							if (gbLight == false)
							{
								glEnable(GL_LIGHTING);
								gbLight = true;
							}
							else
							{
								glDisable(GL_LIGHTING);
								gbLight = false;
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
	
	XStoreName(gpDisplay, gWindow, "Rushikesh Vinod Ghatage : Teapot With Static Rotation");
	
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
	GLuint loadBitmapAsTexture(const char*);

	//Code
	gGlxContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGlxContext);

	//SetClearDepth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Texture
	Texture_Marble = loadBitmapAsTexture("Marble.bmp");

	//Light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	glEnable(GL_LIGHT0);

	Resize(giWindowWidth, giWindowHeight);
}

GLuint loadBitmapAsTexture(const char* path)
{
	//Variable Declaration
	int width = 0;
	int height = 0;
	unsigned char* imageData = NULL;
	GLuint textureId;

	//Code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);

	//OpenGL Actual Texture Code
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	//Setting Texture Parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Push Data into Graphic Memory with help of Graphic Driver
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	SOIL_free_image_data(imageData);
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

	glTranslatef(0.0f, 0.0f, -1.0f);
	glRotatef(Angle_rvg, 0.0, 1.0, 0.0);
	
	glBegin(GL_TRIANGLES);	
	for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int vi = face_indicies[i][j];
			int ni = face_indicies[i][j + 3];
			int ti = face_indicies[i][j + 6];

			glNormal3f((GLfloat)normals[ni][0], (GLfloat)normals[ni][1], (GLfloat)normals[ni][2]);
			glTexCoord2f((GLfloat)textures[ti][0], (GLfloat)textures[ti][1]);
			glVertex3f((GLfloat)vertices[vi][0], (GLfloat)vertices[vi][1], (GLfloat)vertices[vi][2]);
		}
	}
	glEnd();

	Angle_rvg += 0.5f;

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update()
{
	//Code
	if (Angle_rvg >= 360.0f)
	{
		Angle_rvg = 0.0f;
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

	if (Texture_Marble)
	{
		glDeleteTextures(1, &Texture_Marble);
	}
}


