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

struct Light_rvg
{
	vec4 lightAmbient_rvg;
	vec4 lightDiffuse_rvg;
	vec4 lightSpecular_rvg;
	vec4 lightPosition_rvg;
};
struct Light_rvg light_rvg[3];

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

GLfloat sphere_vertices_rvg[1146];
GLfloat sphere_normals_rvg[1146];
GLfloat sphere_textures_rvg[764];
unsigned short sphere_elements_rvg[2280];
GLuint gNumVertices_rvg;
GLuint gNumElements_rvg;

bool bPerVertexLight_rvg = true;
bool bPerFragmentLight_rvg = false;

bool bLight_rvg;

GLfloat materialAmbient_rvg[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse_rvg[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular_rvg[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess_rvg = 50.0f;

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

						case XK_V:
						case XK_v:
							bPerVertexLight_rvg = true;
							bPerFragmentLight_rvg = false;
							break;

						case XK_F:
						case XK_f:
							bPerFragmentLight_rvg = true;
							bPerVertexLight_rvg = false;
							break;

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
							
						case XK_J:
						case XK_j:
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

	XStoreName(gpDisplay_rvg, gWindow_rvg, "Rushikesh Vinod Ghatage : PerVertex & PerFragment Toggling On Sphere");
	
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
	const GLchar *VertexShaderCode_pv_rvg = 
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
	glShaderSource(gVertexShaderObject_pv_rvg, 1, (const GLchar **) &VertexShaderCode_pv_rvg, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pv_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gVertexShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gVertexShaderObject_pv_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "PER VERTEX Vertex Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Vertex Shader Compilation is done Successfully.\n");
	}


	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pv_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pv_rvg = 
									"#version 460 core" \
									"\n" \
									"in vec3 fong_ads_light_pv;" \
									"out vec4 fragColor;" \
									"void main(void)" \
									"{"	\
										"fragColor = vec4(fong_ads_light_pv, 1.0);" \
									"}";
	//Feed to Shader Object
	glShaderSource(gFragmentShaderObject_pv_rvg, 1, (const GLchar **) &FragmentShaderCode_pv_rvg, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pv_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gFragmentShaderObject_pv_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "PER VERTEX Fragment Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Fragment Shader Compilation is done Successfully.\n");
	}	

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	gShaderProgramObject_pv_rvg = glCreateProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	glAttachShader(gShaderProgramObject_pv_rvg, gVertexShaderObject_pv_rvg);		//Vertex Shader
	glAttachShader(gShaderProgramObject_pv_rvg, gFragmentShaderObject_pv_rvg);	//Fragment Shader

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_POSITION, "vPosition_pv");
	glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

	//4)Then, Link all those attached Shader at once.
	glLinkProgram(gShaderProgramObject_pv_rvg);	

	//Shader Linkage Error Checking
	infoLogLength_rvg = 0;
	GLint shaderProgramLinkStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetProgramiv(gShaderProgramObject_pv_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus_rvg);
	if (shaderProgramLinkStatus_rvg == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetProgramInfoLog(gShaderProgramObject_pv_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "SPER VERTEX Shader Program Link Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER VERTEX Program Shader Object Compilation is done Successfully.\n");
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
	const GLchar *VertexShaderCode_pf_rvg = 
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
	glShaderSource(gVertexShaderObject_pf_rvg, 1, (const GLchar **) &VertexShaderCode_pf_rvg, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_pf_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gVertexShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gVertexShaderObject_pf_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "PER FRAGMENT Vertex Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Vertex Shader Compilation is done Successfully.\n");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	gFragmentShaderObject_pf_rvg = glCreateShader(GL_FRAGMENT_SHADER);

	//Provide Source Code to Shader
	const GLchar *FragmentShaderCode_pf_rvg = 
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
	glShaderSource(gFragmentShaderObject_pf_rvg, 1, (const GLchar **) &FragmentShaderCode_pf_rvg, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_pf_rvg);	//Here there should be Shader Compilation Error Checking
	
	//Shader Compilation Error Checking
	infoLogLength_rvg = 0;
	shaderCompilationStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus_rvg);
	if (shaderCompilationStatus_rvg == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetShaderInfoLog(gFragmentShaderObject_pf_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "PER FRAGMENT Fragment Shader Compilation Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Fragment Shader Compilation is done Successfully.\n");
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
	infoLogLength_rvg = 0;
	shaderProgramLinkStatus_rvg = 0;
	szBuffer_rvg = NULL;

	glGetProgramiv(gShaderProgramObject_pf_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus_rvg);
	if (shaderProgramLinkStatus_rvg == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength_rvg);
		if (infoLogLength_rvg > 0)
		{
			szBuffer_rvg = (GLchar *)malloc(infoLogLength_rvg);
			if (szBuffer_rvg != NULL)
			{
				GLsizei written_rvg;
				glGetProgramInfoLog(gShaderProgramObject_pf_rvg, infoLogLength_rvg, &written_rvg, szBuffer_rvg);
				fprintf(gpFile_rvg, "PER FRAGMENT Shader Program Link Log : %s\n", szBuffer_rvg);
				free(szBuffer_rvg);
				Uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile_rvg, "PER FRAGMENT Program Shader Object Compilation is done Successfully.\n");
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
	getSphereVertexData(sphere_vertices_rvg, sphere_normals_rvg, sphere_textures_rvg, sphere_elements_rvg);
	gNumVertices_rvg = getNumberOfSphereVertices();
	gNumElements_rvg = getNumberOfSphereElements();	

		//Create Cassette to use Buffer Array in Display()
	glGenVertexArrays(1, &vao_sphere_rvg);
	glBindVertexArray(vao_sphere_rvg);
			/////////////////////////// For POSITION ///////////////////////////
			//Generate Buffer Array
			glGenBuffers(1, &vbo_sphere_position_rvg);
 
			//Bind Buffer Array
			glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position_rvg);

			//Fill Data into Buffer Array
			glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices_rvg), sphere_vertices_rvg, GL_STATIC_DRAW);

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
			glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals_rvg), sphere_normals_rvg, GL_STATIC_DRAW);

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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements_rvg), sphere_elements_rvg, GL_STATIC_DRAW);

			//Unbind Buffer Array
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Stop Creating Cassette
	glBindVertexArray(0);

	//Stop Creating Cassette
	glBindVertexArray(0);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Draw(void)
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

	//Set modelViewMatrix and projectionMatrix to Identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	modelMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	projectionMatrix = perspectiveProjectionMatrix_rvg;

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

	//Play Cassette
	glBindVertexArray(vao_sphere_rvg);

	//Draw Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
	    glDrawElements(GL_TRIANGLES, gNumElements_rvg, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Stop Playing Cassette
	glBindVertexArray(0);

	//Stop use Shader Program Object
	glUseProgram(0);

	glXSwapBuffers(gpDisplay_rvg, gWindow_rvg);
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

	if (vbo_sphere_element_rvg)
	{
		glDeleteBuffers(1, &vbo_sphere_element_rvg);
		vbo_sphere_element_rvg = 0;
	}

	///////////////////////// Safe Shader Cleanup /////////////////////////
	if (gShaderProgramObject_pv_rvg)
	{
		glUseProgram(gShaderProgramObject_pv_rvg);
		
		glGetProgramiv(gShaderProgramObject_pv_rvg, GL_ATTACHED_SHADERS, &shaderCount_rvg);

		pShader_rvg = (GLuint *)malloc(sizeof(GLuint) * shaderCount_rvg);

		glGetAttachedShaders(gShaderProgramObject_pv_rvg, shaderCount_rvg, &shaderCount_rvg, pShader_rvg);

		for (GLsizei i = 0; i < shaderCount_rvg; i++)
		{
			glDetachShader(gShaderProgramObject_pv_rvg, pShader_rvg[i]);
			glDeleteShader(pShader_rvg[i]);
			pShader_rvg[i] = 0;
		}
		free(pShader_rvg);

		glDeleteProgram(gShaderProgramObject_pv_rvg);
		gShaderProgramObject_pv_rvg = 0;
		glUseProgram(0);
	}

}



