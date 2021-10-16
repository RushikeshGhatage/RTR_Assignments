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

        [window setTitle: @"RVG : macOS Tweaked Smiley"];
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
        
        GLfloat PAngle_rvg;

        GLuint Smiley_Texture;
        unsigned int PressedDigit;
        GLuint defaultColorUniform;
        
        GLuint textureSamplerUniform;

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

        PressedDigit = 0;
        
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
                                        "uniform int u_whiteColor;" \
                                        "uniform sampler2D u_texture_sampler;" \
                                        "out vec4 fragColor;" \
                                        "void main(void)" \
                                        "{" \
                                            "if(u_whiteColor == 1)" \
                                            "{" \
                                                "fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
                                            "}"\
                                            "else" \
                                            "{" \
                                                "fragColor = texture(u_texture_sampler, out_Texcoord);" \
                                            "}" \
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
        defaultColorUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_whiteColor");

        //Vertices Array Declaration
        const GLfloat planeVertices_rvg[] =
        {
            1.0f, 1.0f, 0.0f,       //Front
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };

        //Create Cassette for SQUARE to use Buffer Array in Display()
        glGenVertexArrays(1, &vao_plane);
        glBindVertexArray(vao_plane);
                /////////////////////////////// FOR VERTEX ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_position_plane);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plane);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices_rvg), planeVertices_rvg, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                /////////////////////////////// FOR COLOR ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_texture_plane);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_plane);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, (4 * 2 * sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);

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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            
        //Set Texture
        glEnable(GL_TEXTURE_2D);
        
        Smiley_Texture = [self loadGLTexture:"Smiley.bmp"];
        if (Smiley_Texture == 0)
        {
            fprintf(gpFile_rvg, "Smiley Texture Failed!\n");
            [self release];
            [NSApp terminate:self];
        }
        
        //Set Orthographic Matrix to Identity
        perspectiveProjectionMatrix_rvg = mat4::identity();
        
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
        CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
        CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
        CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
        CVDisplayLinkStart(displayLink);
    }

    -(GLuint)loadGLTexture:(const char *) imageFileName
    {
        //Code
        GLuint textureId;
        
        NSBundle *appBundle = [NSBundle mainBundle];
        NSString *appDirPath = [appBundle bundlePath];
        NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
        NSString *filenameWithPath = [NSString stringWithFormat:@"%@/%s", parentDirPath, imageFileName];
        
        //Get NSImage representation of File
        NSImage *bmpImage = [[NSImage alloc] initWithContentsOfFile:filenameWithPath];
        //Error Changing
        if (!bmpImage)
        {
            fprintf(gpFile_rvg, "NSImage Conversion of file is Failed!\n");
            return(0);
        }
        
        //Get cgImage representation from NSImage
        CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];
        
        //Get width and Height of CGimage
        int width = (int)CGImageGetWidth(cgImage);
        int height = (int)CGImageGetHeight(cgImage);
        
        //Get Core foundation data representation
        CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
        
        //Convert CFDataRef formatted data into void *generic format data
        void *pixels = (void *)CFDataGetBytePtr(imageData);
        
        //OpenGL Actual Texture Code
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        //Setting Texture Parameter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //Push Data into Graphic Memory with help of Graphic Driver
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        CFRelease(imageData);
        return(textureId);
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
        static GLfloat TexCoord[8];
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
        translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

        modelViewProjectionMatrix = translateMatrix;

        modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT

        glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

        //Set Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Smiley_Texture);
        glUniform1i(textureSamplerUniform, 0);

        //Play Cassette of Triangle
        glBindVertexArray(vao_plane);

        if (PressedDigit == 1)
        {
            TexCoord[0] = 1.0f;
            TexCoord[1] = 1.0f;
            
            TexCoord[2] = 0.0f;
            TexCoord[3] = 1.0f;
            
            TexCoord[4] = 0.0f;
            TexCoord[5] = 0.0f;
            
            TexCoord[6] = 1.0f;
            TexCoord[7] = 0.0f;
        }

        else if (PressedDigit == 2)
        {
            TexCoord[0] = 0.5f;
            TexCoord[1] = 0.5f;
            
            TexCoord[2] = 0.0f;
            TexCoord[3] = 0.5f;
            
            TexCoord[4] = 0.0f;
            TexCoord[5] = 0.0f;
            
            TexCoord[6] = 0.5f;
            TexCoord[7] = 0.0f;
        }

        else if (PressedDigit == 3)
        {
            TexCoord[0] = 2.0f;
            TexCoord[1] = 2.0f;
            
            TexCoord[2] = 0.0f;
            TexCoord[3] = 2.0f;
            
            TexCoord[4] = 0.0f;
            TexCoord[5] = 0.0f;
            
            TexCoord[6] = 2.0f;
            TexCoord[7] = 0.0f;
        }

        else if (PressedDigit == 4)
        {
            TexCoord[0] = 0.5f;
            TexCoord[1] = 0.5f;
            
            TexCoord[2] = 0.5f;
            TexCoord[3] = 0.5f;
            
            TexCoord[4] = 0.5f;
            TexCoord[5] = 0.5f;
            
            TexCoord[6] = 0.5f;
            TexCoord[7] = 0.5f;
        }

        if (PressedDigit > 0 && PressedDigit < 5)
        {
            glUniform1i(defaultColorUniform, 0);
        }
        else
        {
            glUniform1i(defaultColorUniform, 1);
        }

        //Bind Buffer Array
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_plane);

        //Fill Data into Buffer Array
        glBufferData(GL_ARRAY_BUFFER, (4 * 2 * sizeof(GLfloat)), TexCoord, GL_DYNAMIC_DRAW);

        //Tell him How to Read Data from Buffer Array
        glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        //Enable RVG_ATTRIBUTE_POSITION Point
        glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

        //Unbind Buffer Array
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Draw Object Triangle
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //Stop Playing Cassette
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
        PAngle_rvg += 1.0f;
        if (PAngle_rvg >= 360.0f)
        {
            PAngle_rvg = 0.0f;
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

            case 1:
            case 49:    //For 1
                PressedDigit = 1;
                break;

            case 50:    //For 2
                PressedDigit = 2;
                break;

            case 51:    //For 3
                PressedDigit = 3;
                break;

            case 52:    //For 4
                PressedDigit = 4;
                break;

            case 'F':
            case 'f':
                [[self window]toggleFullScreen:self];
                break;

            default:
                PressedDigit = -1;
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
        
        if (Smiley_Texture)
        {
            glDeleteTextures(1, &Smiley_Texture);
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

