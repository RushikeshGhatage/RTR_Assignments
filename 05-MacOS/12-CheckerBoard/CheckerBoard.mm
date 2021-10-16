#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //GL.h

#import "vmath.h"

#define CheckImageHeight 64
#define CheckImageWidth 64

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

        [window setTitle: @"RVG : macOS Checker Board"];
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
        
        GLuint vao_plane;                //Vertex Array Objects
        GLuint vbo_position_plane;        //Vertex Buffer Objects for Vertex
        GLuint vbo_texture_plane;            //Vertex Buffer Objects for Colors
        
        GLuint mvpMatrixUniform_rvg;
        GLuint textureSamplerUniform;

        GLubyte checkImage[CheckImageHeight][CheckImageWidth][4];   // Change
        GLuint TexImage;    //Change

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
                                        "in vec2 vTexcoord;" \
                                        "uniform mat4 u_mvpMatrix;" \
                                        "out vec2 out_Texcoord;" \
                                        "void main(void)" \
                                        "{" \
                                            "gl_Position = u_mvpMatrix * vPosition;" \
                                            "out_Texcoord = vTexcoord;" \
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
                                        "in vec2 out_Texcoord;" \
                                        "uniform sampler2D u_texture_sampler;" \
                                        "out vec4 fragColor;" \
                                        "void main(void)" \
                                        "{" \
                                            "fragColor = texture(u_texture_sampler, out_Texcoord);" \
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
        glBindAttribLocation(gShaderProgramObject_rvg, RVG_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
        textureSamplerUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_texture_sampler");

        //Vertices Array Declaration


        //Create Cassette for SQUARE to use Buffer Array in Display()
        glGenVertexArrays(1, &vao_plane);
        glBindVertexArray(vao_plane);
                /////////////////////////////// FOR VERTEX ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_position_plane);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plane);

                //Fill Data into Buffer Array
               glBufferData(GL_ARRAY_BUFFER, (4 * 3 * sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                /////////////////////////////// FOR TEXTURE ///////////////////////////////

                static GLfloat TexCoords[8] = {
                                                0.0f, 0.0f,
                                                0.0f, 1.0f,
                                                1.0f, 1.0f,
                                                1.0f, 0.0f
                                                };
                //Generate Buffer Array
                glGenBuffers(1, &vbo_texture_plane);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_plane);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Stop Creating Cassette for SQUARE
        glBindVertexArray(0);

        //SetClearDepth
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            
        //Set Texture
        glEnable(GL_TEXTURE_2D);
        
        [self loadGLTexture];
        
        //Set Orthographic Matrix to Identity
        perspectiveProjectionMatrix_rvg = mat4::identity();
        
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
        CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
        CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
        CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
        CVDisplayLinkStart(displayLink);
    }

    -(void)loadGLTexture
    {
        [self makeCheckImage];

        //Code
        //OpenGL Actual Texture Code
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &TexImage);
        glBindTexture(GL_TEXTURE_2D, TexImage);

        //Setting Texture Parameter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //Push Data into Graphic Memory with help of Graphic Driver
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CheckImageWidth, CheckImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
    }

    -(void)makeCheckImage
    {
        //Local Variable Declaration
        int i, j, c;

        for (i = 0; i < CheckImageHeight; i++)
        {
            for (j = 0; j < CheckImageWidth; j++)
            {
                c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
                
                checkImage[i][j][0] = (GLubyte)c;
                checkImage[i][j][1] = (GLubyte)c;
                checkImage[i][j][2] = (GLubyte)c;
                checkImage[i][j][3] = 255;
            }
        }
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
        //Local Variable Declaration
        GLfloat StraightQuad[12] = {
                                    -2.0f, -1.0f, 0.0f,
                                    -2.0f, 1.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, -1.0f, 0.0f
                                 };

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

        //Set modelViewMatrix and projectionMatrix to Identity
        modelViewProjectionMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = vmath::translate(0.0f, 0.0f, -3.6f);

        modelViewProjectionMatrix = translateMatrix;

        modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT

        glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TexImage);
        glUniform1i(textureSamplerUniform, 0);

        glBindVertexArray(vao_plane);
                /////////////////////////////// FOR VERTEX ///////////////////////////////
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plane);
            
                    glBufferData(GL_ARRAY_BUFFER, (4 * 3 * sizeof(GLfloat)), StraightQuad, GL_DYNAMIC_DRAW);
            
                    glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            
                    glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);
            
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                StraightQuad[0] = 1.0f;
                StraightQuad[1] = -1.0f;
                StraightQuad[2] = 0.0f;
                
                StraightQuad[3] = 1.0f;
                StraightQuad[4] = 1.0f;
                StraightQuad[5] = 0.0f;
                
                StraightQuad[6] = 2.41421f;
                StraightQuad[7] = 1.0f;
                StraightQuad[8] = -1.41421;

                StraightQuad[9] = 2.41421f;
                StraightQuad[10] = -1.0f;
                StraightQuad[11] = -1.41421;
                
                /////////////////////////////// FOR VERTEX ///////////////////////////////
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plane);
            
                    glBufferData(GL_ARRAY_BUFFER, (4 * 3 * sizeof(GLfloat)), StraightQuad, GL_DYNAMIC_DRAW);
            
                    glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            
                    glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);
            
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);

        //Stop Using Shader Program Object
        glUseProgram(0);
        
        [self updateView];
        
        CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    }

    -(void)updateView
    {
        //Code

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
        
        if (vao_plane)
        {
            glDeleteVertexArrays(1, &vao_plane);
            vao_plane = 0;
        }

        if (vbo_position_plane)
        {
            glDeleteBuffers(1, &vbo_position_plane);
            vbo_position_plane = 0;
        }

        if (vbo_texture_plane)
        {
            glDeleteBuffers(1, &vbo_texture_plane);
            vbo_texture_plane = 0;
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


