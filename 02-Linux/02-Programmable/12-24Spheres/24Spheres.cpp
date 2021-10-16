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
#include "Sphere.h"

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

GLuint LaUniform_rvg;
GLuint LdUniform_rvg;
GLuint LsUniform_rvg;

GLuint lightPositionUniform_rvg;

GLuint modelMatrixUniform_rvg;
GLuint viewMatrixUniform_rvg;
GLuint perspectiveProjectionUniform_rvg;
GLuint LKeyPressedUniform_rvg;

struct Material_rvg
{
	vec4 materialAmbient_rvg;
	vec4 materialDiffuse_rvg;
	vec4 materialSpecular_rvg;
	float materialShininess_rvg;
};
struct Material_rvg material_rvg[24];

struct Uniform_rvg
{
	GLuint KaUniform_rvg;
	GLuint KdUniform_rvg;
	GLuint KsUniform_rvg;
	GLuint shininessUniform_rvg;
};
struct Uniform_rvg uniform_rvg[24];

GLuint gWidth_rvg, gHeight_rvg;

bool bLight_rvg;
bool XKeyPressed_rvg = false;
bool YKeyPressed_rvg = false;
bool ZKeyPressed_rvg = false;

GLfloat lightAngleX_rvg = 0.0f;
GLfloat lightAngleY_rvg = 0.0f;
GLfloat lightAngleZ_rvg = 0.0f;

GLfloat lightAmbient_rvg[] = {0.0f, 0.0f, 0.0f, 1.0f};	//Grey Light
GLfloat lightDiffuse_rvg[] = {1.0f, 1.0f, 1.0f, 1.0f};	//White Light 
GLfloat lightSpecular_rvg[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition_rvg[] = {0.0f, 0.0f, 0.0f, 1.0f};

mat4 perspectiveProjectionMatrix_rvg;

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

						case XK_L:
						case XK_l:
							if (bLight_rvg == true)
							{
								bLight_rvg = false;
							}
							else
							{
								bLight_rvg = true;
							}
							break;

						case XK_X:
						case XK_x:
							XKeyPressed_rvg = true;
							YKeyPressed_rvg = false;
							ZKeyPressed_rvg = false;
							lightAngleX_rvg = 0.0f;
							break;

						case XK_Y:
						case XK_y:
							XKeyPressed_rvg = false;
							YKeyPressed_rvg = true;
							ZKeyPressed_rvg = false;
							lightAngleY_rvg = 0.0f;
							break;

						case XK_Z:
						case XK_z:
							XKeyPressed_rvg = false;
							YKeyPressed_rvg = false;
							ZKeyPressed_rvg = true;
							lightAngleZ_rvg = 0.0f;
							break;							
							
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

	XStoreName(gpDisplay_rvg, gWindow_rvg, "Rushikesh Vinod Ghatage : Per Fragment Light On Sphere");
	
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

	///////////// Material Declaration /////////////

	material_rvg[0].materialAmbient_rvg = vec4(0.215f, 0.1745f, 0.0215f, 1.0f);					// (1)
	material_rvg[0].materialDiffuse_rvg = vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	material_rvg[0].materialSpecular_rvg = vec4(0.633f, 0.727811f, 0.633f, 1.0f);
	material_rvg[0].materialShininess_rvg = 0.6f * 128.0f;

	material_rvg[1].materialAmbient_rvg = vec4(0.135f, 0.2225f, 0.1575f, 1.0f);					// (2)
	material_rvg[1].materialDiffuse_rvg = vec4(0.54f, 0.89f, 0.63f, 1.0f);
	material_rvg[1].materialSpecular_rvg = vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	material_rvg[1].materialShininess_rvg = 0.1f * 128.0f;

	material_rvg[2].materialAmbient_rvg = vec4(0.05375f, 0.05f, 0.0625f, 1.0f);					// (3)
	material_rvg[2].materialDiffuse_rvg = vec4(0.18275f, 0.17f, 0.22525f, 1.0f);
	material_rvg[2].materialSpecular_rvg = vec4(0.332741f, 0.338634f, 0.346435f, 1.0f);
	material_rvg[2].materialShininess_rvg = 0.3f * 128.0f;

	material_rvg[3].materialAmbient_rvg = vec4(0.25f, 0.20725f, 0.20725f, 1.0f);				// (4)
	material_rvg[3].materialDiffuse_rvg = vec4(1.0f, 0.829f, 0.829f, 1.0f);
	material_rvg[3].materialSpecular_rvg = vec4(0.296648f, 0.296648f, 0.296648f, 1.0f);
	material_rvg[3].materialShininess_rvg = 0.088f *128.0f;

	material_rvg[4].materialAmbient_rvg = vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);				// (5)
	material_rvg[4].materialDiffuse_rvg = vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	material_rvg[4].materialSpecular_rvg = vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	material_rvg[4].materialShininess_rvg = 0.6f * 128.0f;

	material_rvg[5].materialAmbient_rvg = vec4(1.0f, 0.18725f, 0.1745f, 1.0f);					// (6)
	material_rvg[5].materialDiffuse_rvg = vec4(0.396f, 0.74191f, 0.69102f, 1.0f);
	material_rvg[5].materialSpecular_rvg = vec4(0.297254f, 0.30829f, 0.306678f, 1.0f);
	material_rvg[5].materialShininess_rvg = 0.1f * 128.0f;

	material_rvg[6].materialAmbient_rvg = vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);			// (7)
	material_rvg[6].materialDiffuse_rvg = vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	material_rvg[6].materialSpecular_rvg = vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	material_rvg[6].materialShininess_rvg = 0.21794872f * 128.0f;

	material_rvg[7].materialAmbient_rvg = vec4(0.2125f, 0.1275f, 0.054f, 1.0f);					// (8)
	material_rvg[7].materialDiffuse_rvg = vec4(0.714f, 0.4284f, 0.18144f, 1.0f);
	material_rvg[7].materialSpecular_rvg = vec4(0.393548f, 0.271906f, 0.163721f, 1.0f);
	material_rvg[7].materialShininess_rvg = 0.2f * 128.0f;

	material_rvg[8].materialAmbient_rvg = vec4(0.25f, 0.25f, 0.25f, 1.0f);						// (9)
	material_rvg[8].materialDiffuse_rvg = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	material_rvg[8].materialSpecular_rvg = vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	material_rvg[8].materialShininess_rvg = 0.6f * 128.0f;

	material_rvg[9].materialAmbient_rvg = vec4(0.19125f, 0.735f, 0.0225f, 1.0f);				// (10)
	material_rvg[9].materialDiffuse_rvg = vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	material_rvg[9].materialSpecular_rvg = vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	material_rvg[9].materialShininess_rvg = 0.1 * 128.0f;

	material_rvg[10].materialAmbient_rvg = vec4(0.24725f, 0.1795f, 0.0745f, 1.0f);				// (11)
	material_rvg[10].materialDiffuse_rvg = vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	material_rvg[10].materialSpecular_rvg = vec4(0.658281f, 0.555802f, 0.366065f, 1.0f);
	material_rvg[10].materialShininess_rvg = 0.4f * 128.0f;

	material_rvg[11].materialAmbient_rvg = vec4(0.19225f, 0.19225f, 0.19225f, 1.0f);			// (12)
	material_rvg[11].materialDiffuse_rvg = vec4(0.50754f, 0.50754f, 0.50754f, 1.0f);
	material_rvg[11].materialSpecular_rvg = vec4(0.508273f, 0.508273f, 0.508273f, 1.0f);
	material_rvg[11].materialShininess_rvg = 0.4f * 128.0f;

	material_rvg[12].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);						// (13)
	material_rvg[12].materialDiffuse_rvg = vec4(0.01f, 0.01f, 0.01f, 1.0f);
	material_rvg[12].materialSpecular_rvg = vec4(0.50f, 0.50f, 0.50f, 1.0f);
	material_rvg[12].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[13].materialAmbient_rvg = vec4(0.0f, 0.1f, 0.06f, 1.0f);						// (14)
	material_rvg[13].materialDiffuse_rvg = vec4(0.0f, 0.50960392f, 0.50960392f, 1.0f);
	material_rvg[13].materialSpecular_rvg = vec4(0.50196078f, 0.50196078f, 0.50196078f, 1.0f);
	material_rvg[13].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[14].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);						// (15)
	material_rvg[14].materialDiffuse_rvg = vec4(0.1f, 0.35f, 0.1f, 1.0f);
	material_rvg[14].materialSpecular_rvg = vec4(0.45f, 0.55f, 0.45f, 1.0f);
	material_rvg[14].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[15].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);						// (16)
	material_rvg[15].materialDiffuse_rvg = vec4(0.5f, 0.0f, 0.0f, 1.0f);
	material_rvg[15].materialSpecular_rvg = vec4(0.7f, 0.6f, 0.6f, 1.0f);
	material_rvg[15].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[16].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);						// (17)
	material_rvg[16].materialDiffuse_rvg = vec4(0.55f, 0.55f, 0.55f, 1.0f);
	material_rvg[16].materialSpecular_rvg = vec4(0.70f, 0.70f, 0.70f, 1.0f);
	material_rvg[16].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[17].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);						// (18)
	material_rvg[17].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.0f, 1.0f);
	material_rvg[17].materialSpecular_rvg = vec4(0.60f, 0.60f, 0.50f, 1.0f);
	material_rvg[17].materialShininess_rvg = 0.25f * 128.0f;

	material_rvg[18].materialAmbient_rvg = vec4(0.02f, 0.02f, 0.02f, 1.0f);						// (19)
	material_rvg[18].materialDiffuse_rvg = vec4(0.01f, 0.01f, 0.01f, 1.0f);
	material_rvg[18].materialSpecular_rvg = vec4(0.40f, 0.40f, 0.40f, 1.0f);
	material_rvg[18].materialShininess_rvg = 0.078125f * 128.0f;

	material_rvg[19].materialAmbient_rvg = vec4(0.0f, 0.05f, 0.05f, 1.0f);						// (20)
	material_rvg[19].materialDiffuse_rvg = vec4(0.4f, 0.5f, 0.5f, 1.0f);
	material_rvg[19].materialSpecular_rvg = vec4(0.04f, 0.70f, 0.70f, 1.0f);
	material_rvg[19].materialShininess_rvg = 0.078125f * 128.0f;

	material_rvg[20].materialAmbient_rvg = vec4(0.0f, 0.05f, 0.0f, 1.0f);						// (21)
	material_rvg[20].materialDiffuse_rvg = vec4(0.4f, 0.5f, 0.4f, 1.0f);
	material_rvg[20].materialSpecular_rvg = vec4(0.04f, 0.7f, 0.04f, 1.0f);
	material_rvg[20].materialShininess_rvg = 0.078125f * 128.0f;

	material_rvg[21].materialAmbient_rvg = vec4(0.05f, 0.0f, 0.0f, 1.0f);						// (22)
	material_rvg[21].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.4f, 1.0f);
	material_rvg[21].materialSpecular_rvg = vec4(0.7f, 0.04f, 0.06f, 1.0f);
	material_rvg[21].materialShininess_rvg = 0.078125f *  128.0f;

	material_rvg[22].materialAmbient_rvg = vec4(0.05f, 0.05f, 0.05f, 1.0f);						// (23)
	material_rvg[22].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	material_rvg[22].materialSpecular_rvg = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	material_rvg[22].materialShininess_rvg = 0.078125f * 128.0f;

	material_rvg[23].materialAmbient_rvg = vec4(0.05f, 0.05f, 0.0f, 1.0f);						// (24)
	material_rvg[23].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.4f, 1.0f);
	material_rvg[23].materialSpecular_rvg = vec4(0.7f, 0.7f, 0.04f, 1.0f);
	material_rvg[23].materialShininess_rvg = 0.078125f * 128.0f;

	////////////////////////////////////////////////

	// ****************** Vertex Shader ******************
	//Create Shader
	gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source Code to Shader
	const GLchar *VertexShaderCode = 
									"#version 460 core" \
									"\n" \
									"in vec4 vPosition;" \
									"in vec3 vNormal;" \

									"out vec3 transformed_normal;" \
									"out vec3 light_direction;" \
									"out vec3 view_vector;" \
									
									"uniform vec4 u_light_position;" \
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
											"light_direction = vec3(u_light_position - eye_coordinates);" \
											"view_vector = (-eye_coordinates.xyz);" \
										"}" \
										"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
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

									"in vec3 transformed_normal;" \
									"in vec3 light_direction;" \
									"in vec3 view_vector;" \
									
									"uniform vec3 u_la;" \
									"uniform vec3 u_ld;" \
									"uniform vec3 u_ls;" \

									"uniform vec3 u_ka;" \
									"uniform vec3 u_kd;" \
									"uniform vec3 u_ks;" \
									"uniform float u_materialShininess;" \
									"out vec3 fong_ads_light;" \

									"uniform int u_lKeyPressed;" \

									"void main(void)" \
									"{"	\

									"if(u_lKeyPressed == 1)" \
										"{" \
											"vec3 normalized_tranformed_normal = normalize(transformed_normal);" \
											"vec3 normalized_light_direction = normalize(light_direction);" \
											"vec3 normalized_view_vector = normalize(view_vector);"
											"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" \
											"vec3 ambient = u_la * u_ka;" \
											"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" \
											"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess);" \
											"fong_ads_light = ambient + diffuse + specular;" \
										"}" \
										"else" \
										"{" \
											"fong_ads_light = vec3(1.0, 1.0, 1.0);" \
										"}" \
										"fragColor = vec4(fong_ads_light, 1.0);" \
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
	glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal");

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
	LaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la");
	LdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld");
	LsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls");
	lightPositionUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position");

	for (int i = 0; i < 24; i++)
	{
		uniform_rvg[i].KaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ka");
		uniform_rvg[i].KdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_kd");
		uniform_rvg[i].KsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ks");
		uniform_rvg[i].shininessUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_materialShininess");
	}

	LKeyPressedUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_lKeyPressed");

	modelMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_modelMatrix");
	viewMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_viewMatrix");
	perspectiveProjectionUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_projectionMatrix");

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

	//SetClearColor
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	bLight_rvg = false;

	//SetClearDepth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	perspectiveProjectionMatrix_rvg = mat4::identity();

	Resize(giWindowWidth_rvg, giWindowHeight_rvg);
}

void Resize(int width, int height)
{
	//Code
	if (height == 0)
	{
		height = 1;
	}

	gWidth_rvg = width;
	gHeight_rvg = height;

	// glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Draw(void)
{
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	glUseProgram(gShaderProgramObject_rvg);

	if (bLight_rvg == true)
	{
		if (XKeyPressed_rvg == true)
		{
			lightPosition_rvg[0] = 0.0f;
			lightPosition_rvg[1] = 28.0f * sin(lightAngleX_rvg);
			lightPosition_rvg[2] = 28.0f * cos(lightAngleX_rvg);
			lightPosition_rvg[3] = 1.0f;
		}

		else if (YKeyPressed_rvg == true)
		{
			lightPosition_rvg[0] = 28.0f * sin(lightAngleY_rvg);
			lightPosition_rvg[1] = 0.0f;
			lightPosition_rvg[2] = 28.0f * cos(lightAngleY_rvg);
			lightPosition_rvg[3] = 1.0f;
		}

		else if (ZKeyPressed_rvg == true)
		{
			lightPosition_rvg[0] = 28.0f * sin(lightAngleZ_rvg);
			lightPosition_rvg[1] = 28.0f * cos(lightAngleZ_rvg);
			lightPosition_rvg[2] = 0.0f;
			lightPosition_rvg[3] = 1.0f;
		}
		
		glUniform1i(LKeyPressedUniform_rvg, 1);
		
		glUniform3fv(LaUniform_rvg, 1, lightAmbient_rvg);
		glUniform3fv(LdUniform_rvg, 1, lightDiffuse_rvg);
		glUniform3fv(LsUniform_rvg, 1, lightSpecular_rvg);


		glUniform4fv(lightPositionUniform_rvg, 1, lightPosition_rvg);
	}
	else
	{
		glUniform1i(LKeyPressedUniform_rvg, 0);
	}

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;

	int iMaterial_rvg = 0;
	
	for (float i = 0.0f; i < gHeight_rvg; i = i + gHeight_rvg / 4.0f)
	{
		for (float j = 0.0f; j < gWidth_rvg; j = j + gWidth_rvg / 6.0f)
		{
			glUniform3fv(uniform_rvg[iMaterial_rvg].KaUniform_rvg, 1, material_rvg[iMaterial_rvg].materialAmbient_rvg);
			glUniform3fv(uniform_rvg[iMaterial_rvg].KdUniform_rvg, 1, material_rvg[iMaterial_rvg].materialDiffuse_rvg);
			glUniform3fv(uniform_rvg[iMaterial_rvg].KsUniform_rvg, 1, material_rvg[iMaterial_rvg].materialSpecular_rvg);

			glUniform1f(uniform_rvg[iMaterial_rvg].shininessUniform_rvg, material_rvg[iMaterial_rvg].materialShininess_rvg);

			glViewport((GLsizei)j, (GLsizei)i, (GLsizei)gWidth_rvg / 5.5, (GLsizei)gHeight_rvg / 5.0);
			perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)gWidth_rvg / (GLfloat)gHeight_rvg, 0.1f, 100.0f);

			modelMatrix = mat4::identity();
			viewMatrix = mat4::identity();
			projectionMatrix = mat4::identity();

			modelMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
			projectionMatrix = perspectiveProjectionMatrix_rvg;

	    	glUniformMatrix4fv(modelMatrixUniform_rvg, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform_rvg, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(perspectiveProjectionUniform_rvg, 1, GL_FALSE, projectionMatrix);		

			//Play Cassette
			glBindVertexArray(vao_sphere);

			//Draw Object
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	    		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	    	//Stop Playing Cassette
			glBindVertexArray(0);

			iMaterial_rvg++;
		}
	}

	//Stop use Shader Program Object
	glUseProgram(0);

	glXSwapBuffers(gpDisplay_rvg, gWindow_rvg);
}

void Update()
{
	//Code
	if (bLight_rvg == true)
	{
		if (XKeyPressed_rvg == true)
		{
			lightAngleX_rvg += 0.03f;
		}

		if (YKeyPressed_rvg == true)
		{
			lightAngleY_rvg += 0.03f;
		}

		if (ZKeyPressed_rvg == true)
		{
			lightAngleZ_rvg += 0.03f;
		}

		if (lightAngleX_rvg >= 360.0f)
		{
			lightAngleX_rvg = 0.0f;
			lightAngleY_rvg = 0.0f;
			lightAngleZ_rvg = 0.0f;
		}
	}
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



