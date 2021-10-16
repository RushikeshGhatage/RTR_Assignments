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

        [window setTitle: @"RVG : macOS Multi Color Pyramid Cube"];
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
        
        GLuint vao_pyramid;                //Vertex Array Objects
        GLuint vbo_position_pyramid;    //Vertex Buffer Objects for Vertex
        GLuint vbo_color_pyramid;        //Vertex Buffer Objects for Colors

        GLuint vao_cube;                //Vertex Array Objects
        GLuint vbo_position_cube;        //Vertex Buffer Objects for Vertex
        GLuint vbo_color_cube;            //Vertex Buffer Objects for Colors
        
        GLuint mvpMatrixUniform_rvg;
        
        GLfloat PAngle_rvg;
        GLfloat CAngle_rvg;

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
        
        //Shader Code
        // ****************** Vertex Shader ******************
        //Create Shader
        gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

        //Provide Source Code to Shader
        const GLchar *VertexShaderCode =
                                        "#version 410 core" \
                                        "\n" \
                                        "in vec4 vPosition;" \
                                        "in vec4 vColor;" \
                                        "uniform mat4 u_mvpMatrix;" \
                                        "out vec4 out_color;" \
                                        "void main(void)" \
                                        "{"    \
                                            "gl_Position = u_mvpMatrix * vPosition;" \
                                            "out_color = vColor;" \
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
                                        "in vec4 out_color;" \
                                        "out vec4 fragColor;" \
                                        "void main(void)" \
                                        "{"    \
                                            "fragColor = out_color;" \
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
        glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_COLOR, "vColor");

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
        mvpMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_mvpMatrix");

        //Vertices Array Declaration
        //Vertices Array Declaration
        const GLfloat pyramidVertices[] =
        {
            0.0f, 0.75f, 0.0f,         //Apex
            -0.5f, -0.25f, 0.5f,    //Front Left Bottom
            0.5f, -0.25f, 0.5f,     //Front Right Bottom

            0.0f, 0.75f, 0.0f,        //Apex
            0.5f, -0.25f, 0.5f,        //Right Left Bottom
            0.5f, -0.25f, -0.5f,    //Right Right Bottom

            0.0f, 0.75f, 0.0f,        //Apex
            0.5f, -0.25f, -0.5f,    //Back Left Bottom
            -0.5f, -0.25f, -0.5f,    //Back Right Bottom

            0.0f, 0.75f, 0.0f,        //Apex
            -0.5f, -0.25f, -0.5f,    //Left Left Bottom
            -0.5f, -0.25f, 0.5f        //Left Right Bottom
        };

        const GLfloat pyramidColors[] =
        {
            1.0f, 0.0f, 0.0f,    //Red         //Front
            0.0f, 1.0f, 0.0f,    //Green
            0.0f, 0.0f, 1.0f,    //Blue

            1.0f, 0.0f, 0.0f,    //Red         //Right
            0.0f, 0.0f, 1.0f,    //Blue
            0.0f, 1.0f, 0.0f,    //Green

            1.0f, 0.0f, 0.0f,    //Red         //Back
            0.0f, 1.0f, 0.0f,    //Green
            0.0f, 0.0f, 1.0f,    //Blue

            1.0f, 0.0f, 0.0f,    //Red         //Left
            0.0f, 0.0f, 1.0f,    //Blue
            0.0f, 1.0f, 0.0f    //Green
        };

        const GLfloat cubeVertices[] =
        {
            0.5f, 0.5f, 0.5f,        //Front Top Right
            -0.5f, 0.5f, 0.5f,        //Front Top Left
            -0.5f, -0.5f, 0.5f,        //Front Bottom Left
            0.5f, -0.5f, 0.5f,        //Front Bottom Right
            
            0.5f, 0.5f, -0.5f,        //Right Top Right
            0.5f, 0.5f, 0.5f,        //Right Top Left
            0.5f, -0.5f, 0.5f,        //Right Bottom Left
            0.5f, -0.5f, -0.5f,        //Right Bottom Right

            -0.5f, 0.5f, -0.5f,        //Back Top Right
            0.5f, 0.5f, -0.5f,        //Back Top Left
            0.5f, -0.5f, -0.5f,        //Back Bottom Left
            -0.5f, -0.5f, -0.5f,    //Back Bottom Right

            -0.5f, 0.5f, 0.5f,        //Left Top Right
            -0.5f, 0.5f, -0.5f,        //Left Top Left
            -0.5f, -0.5f, -0.5f,    //Left Bottom Left
            -0.5f, -0.5f, 0.5f,        //Left Bottom Right

            0.5f, 0.5f, -0.5f,        //Top Top Right
            -0.5f, 0.5f, -0.5f,        //Top Top Left
            -0.5f, 0.5f, 0.5f,        //Top Bottom Left
            0.5f, 0.5f, 0.5f,        //Top Bottom Right

            0.5f, -0.5f, 0.5f,        //Bottom Top Right
            -0.5f, -0.5f, 0.5f,        //Bottom Top Left
            -0.5f, -0.5f, -0.5f,    //Bottom Bottom Left
            0.5f, -0.5f, -0.5f        //Bottom Bottom Right
        };

        const GLfloat cubeColors[] =
        {
            1.0f, 0.0f, 0.0f,    //Red         //FRONT
            0.0f, 1.0f, 0.0f,    //Green
            0.0f, 0.0f, 1.0f,    //Blue
            1.0f, 1.0f, 0.0f,    //Yellow

            0.0f, 0.0f, 1.0f,    //Blue         //RIGHT
            1.0f, 0.0f, 0.0f,    //Red
            1.0f, 1.0f, 0.0f,    //Yellow
            0.0f, 1.0f, 0.0f,    //Green
            
            1.0f, 1.0f, 0.0f,    //Yellow    //BACK
            0.0f, 0.0f, 1.0f,    //Blue
            0.0f, 1.0f, 0.0f,    //Green
            1.0f, 0.0f, 0.0f,    //Red

            0.0f, 1.0f, 0.0f,    //Green     //LEFT
            1.0f, 1.0f, 0.0f,    //Yellow
            1.0f, 0.0f, 0.0f,    //Red
            0.0f, 0.0f, 1.0f,    //Blue

            0.0f, 0.0f, 1.0f,    //Blue     //TOP
            1.0f, 1.0f, 0.0f,    //Yellow
            0.0f, 1.0f, 0.0f,    //Green
            1.0f, 0.0f, 0.0f,    //Red

            1.0f, 1.0f, 0.0f,    //Yellow     //BOTTOM
            0.0f, 0.0f, 1.0f,    //Blue
            1.0f, 0.0f, 0.0f,    //Red
            0.0f, 1.0f, 0.0f,    //Green
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

                /////////////////////////////// FOR COLOR ///////////////////////////////
                //Generate Buffer Array
                 glGenBuffers(1, &vbo_color_pyramid);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColors), pyramidColors, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_COLOR);

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
                 glGenBuffers(1, &vbo_color_cube);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_COLOR);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Stop Creating Cassette for SQUARE
        glBindVertexArray(0);

        //SetClearDepth
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
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

        //OpenGL Drawing Code
        mat4 modelViewProjectionMatrix;
        mat4 projectionMatrix;
        mat4 translateMatrix;
        mat4 rotationMatrix;
        mat4 rotationMatrixX, rotationMatrixY, rotationMatrixZ;
        mat4 scaleMatrix;

        //Translation for TRIANGLE
        //Set modelViewMatrix and projectionMatrix to Identity
        modelViewProjectionMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = vmath::translate(-1.25f, -0.2f, -4.0f);
        rotationMatrix = vmath::rotate(PAngle_rvg, 0.0f, 1.0f, 0.0f);

        modelViewProjectionMatrix = translateMatrix * rotationMatrix;

        modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT

        glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

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
        rotationMatrixX = vmath::rotate(CAngle_rvg, 1.0f, 0.0f, 0.0f);
        rotationMatrixY = vmath::rotate(CAngle_rvg, 0.0f, 1.0f, 0.0f);
        rotationMatrixZ = vmath::rotate(CAngle_rvg, 0.0f, 0.0f, 1.0f);
        scaleMatrix = vmath::scale(0.80f, 0.80f, 0.80f);

        modelViewProjectionMatrix = translateMatrix * scaleMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ;

        modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT

        glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

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
        
        [self updateView];
        
        CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    }

    -(void)updateView
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

        if (vbo_position_pyramid)
        {
            glDeleteBuffers(1, &vbo_position_pyramid);
            vbo_position_pyramid = 0;
        }

        if (vbo_color_pyramid)
        {
            glDeleteBuffers(1, &vbo_color_pyramid);
            vbo_color_pyramid = 0;
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

        if (vbo_color_cube)
        {
            glDeleteBuffers(1, &vbo_color_cube);
            vbo_color_cube = 0;
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
