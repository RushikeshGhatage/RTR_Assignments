#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //GL.h

#import "vmath.h"

#define MAX_rvg 32

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

        [window setTitle: @"RVG : macOS Solar System"];
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

        //const int MAX_rvg;
        
        GLuint gVertexShaderObject_rvg;
        GLuint gFragmentShaderObject_rvg;
        GLuint gShaderProgramObject_rvg;

        GLuint vao_sphere_rvg;                    //Vertex Array Objects
        GLuint vbo_sphere_position_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_normal_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_texture_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_element_rvg;           //Vertex Buffer Objects

        GLuint mvpMatrixUniform_rvg;

        mat4 stack[MAX_rvg];
        int top_rvg;

        GLint day_rvg;
        GLint year_rvg;

        int numElements;
        int numVertices;
        int maxElements;

        float *verts;
        float *norms;
        float *texCoords;
        short *elements;

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
        //Variable Declaration
        top_rvg = -1;
        day_rvg = 0;
        year_rvg = 0;

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
                                        "in vec4 vColor;"
                                        "out vec4 out_color;"
                                        "uniform mat4 u_mvpMatrix;" \
                                        "void main(void)" \
                                        "{" \
                                            "gl_Position = u_mvpMatrix * vPosition;" \
                                            "out_color = vColor;"
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
                                        "in vec4 out_color;"
                                        "out vec4 fragColor;" \
                                        "void main(void)" \
                                        "{" \
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
        [self makeSphereWithRadius:2.0f usingSlices:40 andStacks:40];

        //SetClearDepth
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);        //Hidden Surface Removal
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

    -(void)makeSphereWithRadius:(float)radius usingSlices:(float)slices andStacks:(float)stacks
    {
        //Local Variable Declaration
        float RVG_PI = 3.14159f;
        float drho = (float)RVG_PI / (float)stacks;
        float dtheta = 2.0f * (float)RVG_PI / (float)slices;
        float ds = 1.0f / (float)slices;
        float dt = 1.0f / (float)stacks;
        float t = 1.0f;
        float s = 0.0f;
        int i = 0;
        int j = 0;

        numElements = 0;
        maxElements = slices * stacks * 6;
        numVertices = 0;

        //Code
        float iNumIndices = (maxElements / 3);

        elements = (short *)malloc(iNumIndices * 3 * sizeof(short));
        verts = (float *)malloc(iNumIndices * 3 * sizeof(float));
        norms = (float *)malloc(iNumIndices * 3 * sizeof(float));
        texCoords = (float *)malloc(iNumIndices * 2 * sizeof(float));

        for (int i = 0; i < stacks; i++)
        {
            float rho = (float)(i * drho);
            float srho = (float)sin(rho);
            float crho = (float)cos(rho);
            float srhodrho = (float)sin(rho + drho);
            float crhodrho = (float)cos(rho + drho);

            // Many sources of OpenGL sphere drawing code uses a triangle fan
            // for the caps of the sphere. This however introduces texturing
            // artifacts at the poles on some OpenGL implementations
            s = 0.0f;

            // initialization of three 2-D arrays, two are 4 x 3 and one is 4 x 2
            float vertex[4][3];
            float normal[4][3];
            float texture[4][2];

            for (int j = 0; j < slices; j++)
            {
                float theta = j == slices ? 0.0f : j * dtheta;
                float stheta = (float)-sin(theta);
                float ctheta = (float)cos(theta);
                
                float x = stheta * srho;
                float y = ctheta * srho;
                float z = crho;
                
                texture[0][0] = s;
                texture[0][1] = t;
                normal[0][0] = x;
                normal[0][1] = y;
                normal[0][2] = z;
                vertex[0][0] = x * radius;
                vertex[0][1] = y * radius;
                vertex[0][2] = z * radius;

                x = stheta * srhodrho;
                y = ctheta * srhodrho;
                z = crhodrho;

                texture[1][0] = s;
                texture[1][1] = t - dt;
                normal[1][0] = x;
                normal[1][1] = y;
                normal[1][2] = z;
                vertex[1][0] = x * radius;
                vertex[1][1] = y * radius;
                vertex[1][2] = z * radius;
                
                theta = (j+1) == slices ? 0.0f : (j+1) * dtheta;
                stheta = (float)-sin(theta);
                ctheta = (float)cos(theta);
                
                x = stheta * srho;
                y = ctheta * srho;
                z = crho;
                
                s += ds;
                texture[2][0] = s;
                texture[2][1] = t;
                normal[2][0] = x;
                normal[2][1] = y;
                normal[2][2] = z;
                vertex[2][0] = x * radius;
                vertex[2][1] = y * radius;
                vertex[2][2] = z * radius;
                
                x = stheta * srhodrho;
                y = ctheta * srhodrho;
                z = crhodrho;
                
                texture[3][0] = s;
                texture[3][1] = t - dt;
                normal[3][0] = x;
                normal[3][1] = y;
                normal[3][2] = z;
                vertex[3][0] = x * radius;
                vertex[3][1] = y * radius;
                vertex[3][2] = z * radius;
                
                [self addTriangleWithVertex:vertex usingNormal:normal andTexture:texture];

                // Rearrange for next triangle
                vertex[0][0]=vertex[1][0];
                vertex[0][1]=vertex[1][1];
                vertex[0][2]=vertex[1][2];
                normal[0][0]=normal[1][0];
                normal[0][1]=normal[1][1];
                normal[0][2]=normal[1][2];
                texture[0][0]=texture[1][0];
                texture[0][1]=texture[1][1];
                
                vertex[1][0]=vertex[3][0];
                vertex[1][1]=vertex[3][1];
                vertex[1][2]=vertex[3][2];
                normal[1][0]=normal[3][0];
                normal[1][1]=normal[3][1];
                normal[1][2]=normal[3][2];
                texture[1][0]=texture[3][0];
                texture[1][1]=texture[3][1];
                
                [self addTriangleWithVertex:vertex usingNormal:normal andTexture:texture];
            }
            t -= dt;
        }

        glGenVertexArrays(1, &vao_sphere_rvg);
        glBindVertexArray(vao_sphere_rvg);
                //////////////////////////////////////////// For Position ////////////////////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_sphere_position_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, numElements * 3 * sizeof(float), verts, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                //////////////////////////////////////////// For Normal ////////////////////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_sphere_normal_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, numElements * 3 * sizeof(float), norms, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_NORMAL);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                //////////////////////////////////////////// For Texture ////////////////////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_sphere_texture_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_texture_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, numElements * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                //////////////////////////////////////////// For Elements ////////////////////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_sphere_element_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements * 3 * sizeof(short), elements, GL_STATIC_DRAW);

                //Unbind Buffer Array
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Stop Creating Cassette
        glBindVertexArray(0);

        // after sending data to GPU, now we can free our arrays
        if(elements)
        {
            free(elements);
            elements = NULL;
        }
        
        if(verts)
        {
            free(verts);
            verts = NULL;
        }
        
        if(norms)
        {
            free(norms);
            norms = NULL;
        }
        
        if(texCoords)
        {
            free(texCoords);
            texCoords = NULL;
        }
    }

    -(void)addTriangleWithVertex:(float[4][3])vertex usingNormal:(float[4][3])normal andTexture:(float[4][2])texture
    {
        //Local Variable Declaration
        const float diff = 0.00001;
        int i, j;

        //Code
        // normals should be of unit length
        for (i = 0; i < 3; i++)
        {
            // square the vector length
            float squaredVectorLength=(normal[i][0] * normal[i][0]) + (normal[i][1] * normal[i][1]) + (normal[i][2] * normal[i][2]);
            
            // get square root of above 'squared vector length'
            float squareRootOfSquaredVectorLength = sqrt(squaredVectorLength);
            
            // scale the vector with 1/squareRootOfSquaredVectorLength
            normal[i][0] = normal[i][0] * 1.0 / squareRootOfSquaredVectorLength;
            normal[i][1] = normal[i][1] * 1.0 / squareRootOfSquaredVectorLength;
            normal[i][2] = normal[i][2] * 1.0 / squareRootOfSquaredVectorLength;
        }
        
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < numVertices; j++) //for the first ever iteration of 'j', numVertices will be 0 because of it's initialization in the parameterized constructor
            {
                if ([self isFoundIdenticalWithVal1:verts[j * 3] usingVal2:vertex[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:verts[(j * 3) + 1] usingVal2:vertex[i][1] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:verts[(j * 3) + 2] usingVal2:vertex[i][2] andDiff:diff] &&

                    [self isFoundIdenticalWithVal1:norms[j * 3] usingVal2:normal[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:norms[(j * 3) + 1] usingVal2:normal[i][1] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:norms[(j * 3) + 2] usingVal2:normal[i][2] andDiff:diff] &&

                    [self isFoundIdenticalWithVal1:texCoords[j * 2] usingVal2:texture[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:texCoords[(j * 2) + 1] usingVal2:texture[i][1] andDiff:diff])
                {
                    elements[numElements] = j;
                    numElements++;
                    break;
                }
            }
            
            //If the single vertex, normal and texture do not match with the given, then add the corressponding triangle to the end of the list
            if (j == numVertices && numVertices < maxElements && numElements < maxElements)
            {
                verts[numVertices * 3] = vertex[i][0];
                verts[(numVertices * 3) + 1] = vertex[i][1];
                verts[(numVertices * 3) + 2] = vertex[i][2];
                
                norms[numVertices * 3] = normal[i][0];
                norms[(numVertices * 3) + 1] = normal[i][1];
                norms[(numVertices * 3) + 2] = normal[i][2];
                
                texCoords[numVertices * 2] = texture[i][0];
                texCoords[(numVertices * 2) + 1] = texture[i][1];
                
                elements[numElements] = numVertices; //adding the index to the end of the list of elements/elements
                numElements++; //incrementing the 'end' of the list
                numVertices++; //incrementing coun of vertices
            }
        }
    }

    -(bool)isFoundIdenticalWithVal1:(float)val1 usingVal2:(float)val2 andDiff:(float)diff
    {
        //Code
        if(abs(val1 - val2) < diff)
            return(true);
        else
            return(false);
    }

    -(void)drawRect:(NSRect)dirtyRect
    {
        //Code
        [self drawView];
    }

    -(void)pushMatrixWithMat4:(mat4)matrix4by4
    {
        //Code
        if (top_rvg >= MAX_rvg - 1)
        {
            fprintf(gpFile_rvg, "Overflow Condition");
        }

        else
        {
            top_rvg++;
            stack[top_rvg] = matrix4by4;
        }
    }

    -(mat4)popMatrix
    {
        //Code
        mat4 popData_rvg;
    
        if (top_rvg<= -1)
        {
            fprintf(gpFile_rvg, "Underflow Condition");
        }

        else
        {
            popData_rvg = stack[top_rvg];
            top_rvg--;
        }

        return(popData_rvg);
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
        mat4 YrotateMatrix;
        mat4 DrotateMatrix;
        mat4 scaleMatrix;

        //Set modelViewMatrix and projectionMatrix to Identity
        modelViewProjectionMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = mat4::identity();
        scaleMatrix = mat4::identity();

        translateMatrix = vmath::translate(0.0f, 0.0f, -20.0f);
        modelViewProjectionMatrix = translateMatrix;

        [self pushMatrixWithMat4 : modelViewProjectionMatrix];
            modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT
            glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

            glVertexAttrib3f(RVG_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            //Play Cassette
            glBindVertexArray(vao_sphere_rvg);

            //Draw Object
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            //Stop Playing Cassette
            glBindVertexArray(0);

        modelViewProjectionMatrix = [self popMatrix];


        YrotateMatrix = vmath::rotate((GLfloat)year_rvg, 0.0f, 1.0f, 0.0f);

        [self pushMatrixWithMat4 : modelViewProjectionMatrix];
            DrotateMatrix = vmath::rotate((GLfloat)day_rvg, 0.0f, 1.0f, 0.0f);
            translateMatrix = vmath::translate(7.0f, 0.0f, 0.0f);
            scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);
            modelViewProjectionMatrix = modelViewProjectionMatrix * YrotateMatrix * translateMatrix * scaleMatrix * DrotateMatrix;
            modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT
            glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

            glVertexAttrib3f(RVG_ATTRIBUTE_COLOR, 0.4f, 0.9f, 1.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            //Play Cassette
            glBindVertexArray(vao_sphere_rvg);

            //Draw Object
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            //Stop Playing Cassette
            glBindVertexArray(0);

        modelViewProjectionMatrix = [self popMatrix];

        //Stop using Shader Program Object
        glUseProgram(0);
        
        //update();
        
        CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
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

            case 'D':
                day_rvg = (day_rvg + 3) % 360;
                break;

            case 'd':
                day_rvg = (day_rvg - 3) % 360;
                break;

            case 'Y':
                year_rvg = (year_rvg + 6) % 360;
                break;

            case 'y':
                year_rvg = (year_rvg - 6) % 360;
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

        if (vbo_sphere_texture_rvg)
        {
            glDeleteBuffers(1, &vbo_sphere_texture_rvg);
            vbo_sphere_texture_rvg = 0;
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




