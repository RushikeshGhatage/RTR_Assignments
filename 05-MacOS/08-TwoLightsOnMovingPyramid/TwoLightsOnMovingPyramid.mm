#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //GL.h

#import "vmath.h"

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *,
                               CVOptionFlags, CVOptionFlags *, void *);

FILE *gpFile_rvg = NULL;

using namespace vmath;

@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>

@end

int main(int argc, char *argv[])
{
    //Code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];        //Run Loop / Message Loop / Event Loop
    [pool release];
    return(0);
}

@interface MyOpenGLView:NSOpenGLView

@end

@implementation AppDelegate
    {
        @private
        NSWindow *window;
        MyOpenGLView *myOpenGLView;
    }

    -(void)applicationDidFinishLaunching:(NSNotification *)aNotification
    {
        //Code
        NSBundle *appBundle = [NSBundle mainBundle];
        NSString *appDirPath = [appBundle bundlePath];
        NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
        NSString *logFilenameWithPath = [NSString stringWithFormat:@"%@/LogFile.txt", parentDirPath];
        const char *pszLogFilenameWithPath = [logFilenameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

        gpFile_rvg = fopen(pszLogFilenameWithPath, "w");
        if (gpFile_rvg == NULL)
        {
            [self release];
            [NSApp terminate:self];
        }

        fprintf(gpFile_rvg, "Program Started Successfully\n");

        NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
        window = [[NSWindow alloc] initWithContentRect:winRect
                                    styleMask:NSWindowStyleMaskTitled             |
                                                NSWindowStyleMaskClosable         |
                                                NSWindowStyleMaskMiniaturizable   |
                                                NSWindowStyleMaskResizable
                                    backing:NSBackingStoreBuffered
                                    defer:NO];

        [window setTitle: @"RVG : macOS Two Lights On Moving Pyramid"];
        [window center];

        myOpenGLView = [[MyOpenGLView alloc]initWithFrame:winRect];

        [window setContentView:myOpenGLView];
        [window setDelegate:self];
        [window makeKeyAndOrderFront:self];
    }

    -(void)applicationWillTerminate:(NSNotification *)aNotification
    {
        //Code
        if (gpFile_rvg)
        {
            fprintf(gpFile_rvg, "Program Terminated Successfully\n");
        }
        fclose(gpFile_rvg);
        gpFile_rvg = NULL;
    }

    -(void)windowWillClose:(NSNotification *)aNotification
    {
        //Code
        [NSApp terminate:self];
    }

    -(void)dealloc
    {
        //Code
        [myOpenGLView release];
        [window release];
        [super dealloc];
    }
@end

@implementation MyOpenGLView
    {
        @private
        CVDisplayLinkRef displayLink;
        
        enum
        {
            RVG_ATTRIBUTE_POSITION = 0,
            RVG_ATTRIBUTE_COLOR,
            RVG_ATTRIBUTE_NORMAL,
            RVG_ATTRIBUTE_TEXCOORD,
        };
        
        GLuint gVertexShaderObject_rvg;
        GLuint gFragmentShaderObject_rvg;
        GLuint gShaderProgramObject_rvg;

        GLuint vao_pyramid;                    //Vertex Array Objects
        GLuint vbo_pyramid_position;           //Vertex Buffer Objects
        GLuint vbo_pyramid_normal;               //Vertex Buffer Objects for Colors

        struct Light_rvg
        {
            vec4 lightAmbient_rvg;
            vec4 lightDiffuse_rvg;
            vec4 lightSpecular_rvg;
            vec4 lightPosition_rvg;
        };
        struct Light_rvg light_rvg[2];

        GLuint LaUniform0_rvg, LdUniform0_rvg, LsUniform0_rvg;
        GLuint LaUniform1_rvg, LdUniform1_rvg, LsUniform1_rvg;
        GLuint lightPositionUniform0_rvg, lightPositionUniform1_rvg;

        GLuint KaUniform_rvg, KdUniform_rvg, KsUniform_rvg;
        GLint shininessUniform_rvg;

        GLuint modelMatrixUniform;
        GLuint viewMatrixUniform;
        GLuint perspectiveProjectionUniform;
        GLuint LKeyPressedUniform;

        bool bLight_rvg;
        bool bAnimate_rvg;

        GLfloat materialAmbient_rvg[4];
        GLfloat materialDiffuse_rvg[4];
        GLfloat materialSpecular_rvg[4];
        GLfloat materialShininess_rvg;

        GLfloat PAngle_rvg;

        mat4 perspectiveProjectionMatrix_rvg;
    }

    -(id)initWithFrame:(NSRect)frame
    {
        //Code
        self = [super initWithFrame:frame];
        if (self)
        {
            NSOpenGLPixelFormatAttribute attribute[] =
            {
                NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
                NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
                NSOpenGLPFANoRecovery,
                NSOpenGLPFAAccelerated,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFADepthSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADoubleBuffer, 0
            };
            
            NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attribute]autorelease];
            if (pixelFormat == nil)
            {
                fprintf(gpFile_rvg, "Cannot get Pixel Format !!!");
                [self release];
                [NSApp terminate:self];
            }
            
            NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
            [self setPixelFormat:pixelFormat];
            [self setOpenGLContext:glContext];
        }
        return(self);
    }

    -(CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
    {
        //Code
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        [self drawView];
        [pool release];
        return(kCVReturnSuccess);
    }

    -(void)prepareOpenGL
    {
        //Code
        [super prepareOpenGL];
        [[self openGLContext]makeCurrentContext];
        
        //Swap Interval
        GLint swapInt = 1;
        [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

        materialAmbient_rvg[0] = 0.0f;
        materialAmbient_rvg[1] = 0.0f;
        materialAmbient_rvg[2] = 0.0f;
        materialAmbient_rvg[3] = 1.0f;

        materialDiffuse_rvg[0] = 1.0f;
        materialDiffuse_rvg[1] = 1.0f;
        materialDiffuse_rvg[2] = 1.0f;
        materialDiffuse_rvg[3] = 1.0f;

        materialSpecular_rvg[0] = 1.0f;
        materialSpecular_rvg[1] = 1.0f;
        materialSpecular_rvg[2] = 1.0f;
        materialSpecular_rvg[3] = 1.0f;

        materialShininess_rvg = 128.0f;

        light_rvg[0].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        light_rvg[0].lightDiffuse_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        light_rvg[0].lightSpecular_rvg = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        light_rvg[0].lightPosition_rvg = vec4(2.0f, 0.0f, 0.0f, 1.0f);

        light_rvg[1].lightAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        light_rvg[1].lightDiffuse_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        light_rvg[1].lightSpecular_rvg = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        light_rvg[1].lightPosition_rvg = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

        
        //Shader Code
        // ****************** Vertex Shader ******************
        //Create Shader
        gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

        //Provide Source Code to Shader
        const GLchar *VertexShaderCode =
                                        "#version 410 core" \
                                        "\n" \
                                        "in vec4 vPosition;" \
                                        "in vec3 vNormal;" \
                                        "uniform int u_lKeyPressed;" \

                                        "uniform mat4 u_modelMatrix;" \
                                        "uniform mat4 u_viewMatrix;" \
                                        "uniform mat4 u_projectionMatrix;" \

                                        "uniform vec3 u_ka;" \
                                        "uniform vec3 u_kd;" \
                                        "uniform vec3 u_ks;" \
                                        "uniform float u_materialShininess;" \
                                        
                                        "uniform vec3 u_la[2];" \
                                        "uniform vec3 u_ld[2];" \
                                        "uniform vec3 u_ls[2];" \
                                        "uniform vec4 u_light_position[2];" \

                                        "out vec3 fong_ads_light;" \

                                        "void main(void)" \
                                        "{" \
                                            "vec3 ambient[2];" \
                                            "vec3 diffuse[2];" \
                                            "vec3 specular[2];" \
                                            "vec3 light_direction[2];" \
                                            "vec3 reflection_vector[2];" \
                                            "fong_ads_light = vec3(0.0);"

                                            "if(u_lKeyPressed == 1)" \
                                            "{" \
                                                "vec4 eye_coordinates = (u_viewMatrix * u_modelMatrix * vPosition);" \
                                                "vec3 transformed_normal = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
                                                "vec3 view_vector = normalize(-eye_coordinates.xyz);" \

                                                "for (int i = 0; i < 2; i++)" \
                                                "{" \
                                                    "ambient[i] = u_la[i] * u_ka;" \
                                                    "light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" \
                                                    "diffuse[i] = u_ld[i] * u_kd * max(dot(light_direction[i], transformed_normal), 0.0);" \
                                                    "reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
                                                    "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess);" \
                                                    "fong_ads_light = fong_ads_light + ambient[i] + diffuse[i] + specular[i];" \
                                                "}" \
                                            "}" \
                                            "else" \
                                            "{" \
                                                "fong_ads_light = vec3(1.0, 1.0, 1.0);" \
                                            "}" \
                                            "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
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
                    [self release];
                    [NSApp terminate:self];
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
                                        "#version 410 core" \
                                        "\n" \
                                        "in vec3 fong_ads_light;" \
                                        "out vec4 fragColor;" \
                                        "void main(void)" \
                                        "{" \
                                            "fragColor = vec4(fong_ads_light, 1.0);" \
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
                    glGetShaderInfoLog(gVertexShaderObject_rvg, infoLogLength, &written, szBuffer);
                    fprintf(gpFile_rvg, "Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [NSApp terminate:self];
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
        glAttachShader(gShaderProgramObject_rvg, gVertexShaderObject_rvg);        //Vertex Shader
        glAttachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);    //Fragment Shader

        //3)(Pre-Linking) Bind with Shader Program Object with Attributes
        glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal");

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
                    [self release];
                    [NSApp terminate:self];
                }
            }
        }
        else
        {
            fprintf(gpFile_rvg, "Shader Program Object Compilation is done Successfully.\n");
        }

        //Get MVP Uniform Location
        LaUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la[0]");
        LdUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[0]");
        LsUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[0]");
        lightPositionUniform0_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[0]");

        LaUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la[1]");
        LdUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[1]");
        LsUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[1]");
        lightPositionUniform1_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[1]");

        KaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ka");
        KdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_kd");
        KsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ks");
        shininessUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_materialShininess");

        LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_lKeyPressed");

        modelMatrixUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_modelMatrix");
        viewMatrixUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_viewMatrix");
        perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_projectionMatrix");

        //Vertices Array Declaration
        const GLfloat pyramidVertices[] =
        {
            0.0f, 1.0f, 0.00f,      //Apex
            -1.0f, -1.0f, 1.0f,     //Front Left Bottom
            1.0f, -1.0f, 1.0f,      //Front Right Bottom
            
            0.0f, 1.0f, 0.0f,       //Apex
            1.0f, -1.0f, -1.0f,     //Back Right Bottom
            -1.0f, -1.0f, -1.0f,    //Back Left Bottom

            0.0f, 1.0f, 0.0f,       //Apex
            -1.0f, -1.0f, -1.0f,    //Left Left Bottom
            -1.0f, -1.0f, 1.0f,     //Left Right Bottom

            0.0f, 1.0f, 0.0f,       //Apex
            1.0f, -1.0f, 1.0f,      //Right Left Bottom
            1.0f, -1.0f, -1.0f      //Right Right Bottom
        };

        const GLfloat pyramidNormal[] =
        {
            0.0f, 0.447214f, 0.894427f,     //Front
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,

            0.0f, 0.447214f, -0.894427f,    //Back
            0.0f, 0.447214f, -0.894427f,
            0.0f, 0.447214f, -0.894427f,

            -0.894427f, 0.447214f, 0.0f,    //Left
            -0.894427f, 0.447214f, 0.0f,
            -0.894427f, 0.447214f, 0.0f,

            0.894427f, 0.447214f, 0.0f,     //Right
            0.894427f, 0.447214f, 0.0f,
            0.894427f, 0.447214f, 0.0f
        };

        //Create Cassette to use Buffer Array in Display()
        glGenVertexArrays(1, &vao_pyramid);
        glBindVertexArray(vao_pyramid);

                ///////////////////////////// For Vertices /////////////////////////////
        
                //Generate Buffer Array
                glGenBuffers(1, &vbo_pyramid_position);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);
        
                /////////////////////////////// FOR NORMAL ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_pyramid_normal);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormal), pyramidNormal, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_NORMAL);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Stop Creating Cassette
        glBindVertexArray(0);

        //SetClearDepth
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        bLight_rvg = false;
        bAnimate_rvg = false;
        
        //Set Orthographic Matrix to Identity
        perspectiveProjectionMatrix_rvg = mat4::identity();
        
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
        CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
        CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
        CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
        CVDisplayLinkStart(displayLink);
    }

    -(void)reshape
    {
        //Code
        [super reshape];
        CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
        NSRect rect = [self bounds];
        if (rect.size.height < 0)
        {
            rect.size.height = 1;
        }
        
        glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
        
        perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)rect.size.width / (GLfloat)rect.size.height, 0.1f, 100.0f);
        
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    }

    -(void)drawRect:(NSRect)dirtyRect
    {
        //Code
        [self drawView];
    }

    -(void)drawView
    {
        //Code
        [[self openGLContext]makeCurrentContext];
        CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use Shader Program Object
        glUseProgram(gShaderProgramObject_rvg);

        if (bLight_rvg == true)
        {
            glUniform1i(LKeyPressedUniform, 1);
            
            glUniform3fv(LaUniform0_rvg, 1, light_rvg[0].lightAmbient_rvg);
            glUniform3fv(LdUniform0_rvg, 1, light_rvg[0].lightDiffuse_rvg);
            glUniform3fv(LsUniform0_rvg, 1, light_rvg[0].lightSpecular_rvg);
            glUniform4fv(lightPositionUniform0_rvg, 1, light_rvg[0].lightPosition_rvg);

            glUniform3fv(LaUniform1_rvg, 1, light_rvg[1].lightAmbient_rvg);
            glUniform3fv(LdUniform1_rvg, 1, light_rvg[1].lightDiffuse_rvg);
            glUniform3fv(LsUniform1_rvg, 1, light_rvg[1].lightSpecular_rvg);
            glUniform4fv(lightPositionUniform1_rvg, 1, light_rvg[1].lightPosition_rvg);

            glUniform3fv(KaUniform_rvg, 1, materialAmbient_rvg);
            glUniform3fv(KdUniform_rvg, 1, materialDiffuse_rvg);
            glUniform3fv(KsUniform_rvg, 1, materialSpecular_rvg);

            glUniform1f(shininessUniform_rvg, materialShininess_rvg);
        }
        else
        {
            glUniform1i(LKeyPressedUniform, 0);
        }

        //OpenGL Drawing Code
        mat4 modelMatrix;
        mat4 viewMatrix;
        mat4 projectionMatrix;
        mat4 translateMatrix;
        mat4 rotateMatrix;

        //Set modelViewMatrix and projectionMatrix to Identity
        modelMatrix = mat4::identity();
        viewMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = mat4::identity();
        rotateMatrix = mat4::identity();

        translateMatrix = vmath::translate(0.0f, 0.0f, -4.5f);
        rotateMatrix = vmath::rotate(PAngle_rvg, 0.0f, 1.0f, 0.0f);

        modelMatrix = translateMatrix * rotateMatrix;
        projectionMatrix = perspectiveProjectionMatrix_rvg;

        //modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;  //ORDER IS IMPORTANT

        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

        //Play Cassette of Triangle
        glBindVertexArray(vao_pyramid);

        //Draw Object Triangle
        glDrawArrays(GL_TRIANGLES, 0, 12);

        //Stop Playing Cassette
        glBindVertexArray(0);

        //Stop Using Shader PrpgramObject
        glUseProgram(0);
        
        [self updateView];
        
        CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    }

    -(void)updateView
    {
        //Code
        if (bAnimate_rvg == true)
        {
            PAngle_rvg += 1.0f;

            if (PAngle_rvg >= 360.0f)
            {
                PAngle_rvg = 0.0f;
            }
        }
    }

    -(BOOL)acceptsFirstResponder
    {
        //Code
        [[self window] makeFirstResponder: self];
        return(YES);
    }

    -(void)keyDown:(NSEvent *)theEvent
    {
        //Code
        int key = [[theEvent characters]characterAtIndex:0];
        switch (key)
        {
            case 27:
                [self release];
                [NSApp terminate:self];
                break;

            case 'A':
            case 'a':
                if (bAnimate_rvg == true)
                {
                    bAnimate_rvg = false;
                }
                else
                {
                    bAnimate_rvg = true;
                }
                break;

            case 'L':
            case 'l':
                if (bLight_rvg == true)
                {
                    bLight_rvg = false;
                }
                else
                {
                    bLight_rvg = true;
                }
                break;

            case 'F':
            case 'f':
                [[self window]toggleFullScreen:self];
                break;

            default:
                break;
        }
    }

    -(void)mouseDown:(NSEvent *)theEvent
    {
        //Code
    }

    -(void)rightMouseDown:(NSEvent *)theEvent
    {
        //Code
    }

    -(void)otherMouseDown:(NSEvent *)theEvent
    {
        //Code
    }

    -(void)dealloc
    {
        //Code
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        
        if (vao_pyramid)
        {
            glDeleteVertexArrays(1, &vao_pyramid);
            vao_pyramid = 0;
        }

        if (vbo_pyramid_position)
        {
            glDeleteBuffers(1, &vbo_pyramid_position);
            vbo_pyramid_position = 0;
        }
        
        if (vbo_pyramid_normal)
        {
            glDeleteBuffers(1, &vbo_pyramid_normal);
            vbo_pyramid_normal = 0;
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
        [super dealloc];
    }
@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now, const CVTimeStamp *outputTime,
                               CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    //Code
    CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return result;
}

