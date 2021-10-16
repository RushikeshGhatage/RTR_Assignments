//
//  MyView.m
//  Window
//
//  Created by user173485 on 7/5/21.
//  Copyright © 2021 AstroMediComp. All rights reserved.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

using namespace vmath;

#define MAX_rvg 32

@implementation GLESView
{
    @private
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    bool isAnimating;


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

        GLuint vao_sphere_rvg;                    //Vertex Array Objects
        GLuint vbo_sphere_position_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_normal_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_texture_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_element_rvg;           //Vertex Buffer Objects

        GLuint mvpMatrixUniform_rvg;

        mat4 stack[MAX_rvg];
        int top_rvg;

        int Shoulder_rvg;
        int Elbow_rvg;

        int numElements_rvg;
        int numVertices_rvg;
        int maxElements_rvg;

        float *verts_rvg;
        float *norms_rvg;
        float *texCoords_rvg;
        short *elements_rvg;

        bool bOnShoulder;
        bool bOnElbow;

    mat4 perspectiveProjectionMatrix_rvg;
}

-(id)initWithFrame:(CGRect)frame
{
    //Code
        bOnShoulder = true;
        bOnElbow = false;
        top_rvg = -1;
        Shoulder_rvg = 0;
        Elbow_rvg = 0;

    self = [super initWithFrame:frame];
    
    if (self)
    {
        //Code
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
        [eaglLayer setOpaque:YES];
        [eaglLayer setDrawableProperties:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                          kEAGLDrawablePropertyRetainedBacking,
                                          kEAGLColorFormatRGBA8,
                                          kEAGLDrawablePropertyColorFormat,
                                          nil]];
        
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (eaglContext == nil)
        {
            printf("OpenGL-ES Context Creation Failed !!!\n");
            return(nil);
        }
        
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Framebuffer is not Complete in initwithframe !!!\n");
            [self unintialize];
            return(nil);
        }
        
        animationFrameInterval = 60; //From iOS 8.2
        isAnimating = NO;
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Shader Code
        // ****************** Vertex Shader ******************
        //Create Shader
        gVertexShaderObject_rvg = glCreateShader(GL_VERTEX_SHADER);

        //Provide Source Code to Shader
        const GLchar *VertexShaderCode =
                                        "#version 300 es" \
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
                    printf("Vertex Shader Compilation Log : %s\n", szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("Vertex Shader Compilation is done Successfully.\n");
        }

        // ****************** Fragment Shader ******************
        //Create Shader
        gFragmentShaderObject_rvg = glCreateShader(GL_FRAGMENT_SHADER);

        //Provide Source Code to Shader
        const GLchar *FragmentShaderCode =
                                        "#version 300 es" \
                                        "\n" \
                                        "precision highp float;" \
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
                    printf("Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("Fragment Shader Compilation is done Successfully.\n");
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
                    printf("Shader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("Shader Program Object Compilation is done Successfully.\n");
        }

        //Get MVP Uniform Location
        mvpMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_mvpMatrix");

        //Vertices Array Declaration
        [self makeSphereWithRadius:2.0f usingSlices:40 andStacks:40];

        //SetClearDepth
        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);        //Hidden Surface Removal
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //Set Orthographic Matrix to Identity
        perspectiveProjectionMatrix_rvg = mat4::identity();
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubletap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [swipeGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [longPressGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return(self);
}

-(void)makeSphereWithRadius:(float)radius usingSlices:(float)slices andStacks:(float)stacks
    {
        //Local Variable Declaration
        float RVG_PI = 3.14159f;
        float drho_rvg = (float)RVG_PI / (float)stacks;
        float dtheta_rvg = 2.0f * (float)RVG_PI / (float)slices;
        float ds_rvg = 1.0f / (float)slices;
        float dt_rvg = 1.0f / (float)stacks;
        float t_rvg = 1.0f;
        float s_rvg = 0.0f;

        numElements_rvg = 0;
        maxElements_rvg = slices * stacks * 6;
        numVertices_rvg = 0;

        //Code
        float iNumIndices = (maxElements_rvg / 3);

        elements_rvg = (short *)malloc(iNumIndices * 3 * sizeof(short));
        verts_rvg = (float *)malloc(iNumIndices * 3 * sizeof(float));
        norms_rvg = (float *)malloc(iNumIndices * 3 * sizeof(float));
        texCoords_rvg = (float *)malloc(iNumIndices * 2 * sizeof(float));

        for (int i = 0; i < stacks; i++)
        {
            float rho = (float)(i * drho_rvg);
            float srho = (float)sin(rho);
            float crho = (float)cos(rho);
            float srhodrho = (float)sin(rho + drho_rvg);
            float crhodrho = (float)cos(rho + drho_rvg);

            // Many sources of OpenGL sphere drawing code uses a triangle fan
            // for the caps of the sphere. This however introduces texturing
            // artifacts at the poles on some OpenGL implementations
            s_rvg = 0.0f;

            // initialization of three 2-D arrays, two are 4 x 3 and one is 4 x 2
            float vertex[4][3];
            float normal[4][3];
            float texture[4][2];

            for (int j = 0; j < slices; j++)
            {
                float theta = j == slices ? 0.0f : j * dtheta_rvg;
                float stheta = (float)-sin(theta);
                float ctheta = (float)cos(theta);
                
                float x = stheta * srho;
                float y = ctheta * srho;
                float z = crho;
                
                texture[0][0] = s_rvg;
                texture[0][1] = t_rvg;
                normal[0][0] = x;
                normal[0][1] = y;
                normal[0][2] = z;
                vertex[0][0] = x * radius;
                vertex[0][1] = y * radius;
                vertex[0][2] = z * radius;

                x = stheta * srhodrho;
                y = ctheta * srhodrho;
                z = crhodrho;

                texture[1][0] = s_rvg;
                texture[1][1] = t_rvg - dt_rvg;
                normal[1][0] = x;
                normal[1][1] = y;
                normal[1][2] = z;
                vertex[1][0] = x * radius;
                vertex[1][1] = y * radius;
                vertex[1][2] = z * radius;
                
                theta = (j+1) == slices ? 0.0f : (j+1) * dtheta_rvg;
                stheta = (float)-sin(theta);
                ctheta = (float)cos(theta);
                
                x = stheta * srho;
                y = ctheta * srho;
                z = crho;
                
                s_rvg += ds_rvg;
                texture[2][0] = s_rvg;
                texture[2][1] = t_rvg;
                normal[2][0] = x;
                normal[2][1] = y;
                normal[2][2] = z;
                vertex[2][0] = x * radius;
                vertex[2][1] = y * radius;
                vertex[2][2] = z * radius;
                
                x = stheta * srhodrho;
                y = ctheta * srhodrho;
                z = crhodrho;
                
                texture[3][0] = s_rvg;
                texture[3][1] = t_rvg - dt_rvg;
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
            t_rvg -= dt_rvg;
        }

        glGenVertexArrays(1, &vao_sphere_rvg);
        glBindVertexArray(vao_sphere_rvg);
                //////////////////////////////////////////// For Position ////////////////////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_sphere_position_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, numElements_rvg * 3 * sizeof(float), verts_rvg, GL_STATIC_DRAW);

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
                glBufferData(GL_ARRAY_BUFFER, numElements_rvg * 3 * sizeof(float), norms_rvg, GL_STATIC_DRAW);

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
                glBufferData(GL_ARRAY_BUFFER, numElements_rvg * 2 * sizeof(float), texCoords_rvg, GL_STATIC_DRAW);

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
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements_rvg * 3 * sizeof(short), elements_rvg, GL_STATIC_DRAW);

                //Unbind Buffer Array
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Stop Creating Cassette
        glBindVertexArray(0);

        // after sending data to GPU, now we can free our arrays
        if(elements_rvg)
        {
            free(elements_rvg);
            elements_rvg = NULL;
        }
        
        if(verts_rvg)
        {
            free(verts_rvg);
            verts_rvg = NULL;
        }
        
        if(norms_rvg)
        {
            free(norms_rvg);
            norms_rvg = NULL;
        }
        
        if(texCoords_rvg)
        {
            free(texCoords_rvg);
            texCoords_rvg = NULL;
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
            for (j = 0; j < numVertices_rvg; j++) //for the first ever iteration of 'j', numVertices_rvg will be 0 because of it's_rvg initialization in the parameterized constructor
            {
                if ([self isFoundIdenticalWithVal1:verts_rvg[j * 3] usingVal2:vertex[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:verts_rvg[(j * 3) + 1] usingVal2:vertex[i][1] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:verts_rvg[(j * 3) + 2] usingVal2:vertex[i][2] andDiff:diff] &&

                    [self isFoundIdenticalWithVal1:norms_rvg[j * 3] usingVal2:normal[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:norms_rvg[(j * 3) + 1] usingVal2:normal[i][1] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:norms_rvg[(j * 3) + 2] usingVal2:normal[i][2] andDiff:diff] &&

                    [self isFoundIdenticalWithVal1:texCoords_rvg[j * 2] usingVal2:texture[i][0] andDiff:diff] &&
                    [self isFoundIdenticalWithVal1:texCoords_rvg[(j * 2) + 1] usingVal2:texture[i][1] andDiff:diff])
                {
                    elements_rvg[numElements_rvg] = j;
                    numElements_rvg++;
                    break;
                }
            }
            
            //If the single vertex, normal and texture do not match with the given, then add the corressponding triangle to the end of the list
            if (j == numVertices_rvg && numVertices_rvg < maxElements_rvg && numElements_rvg < maxElements_rvg)
            {
                verts_rvg[numVertices_rvg * 3] = vertex[i][0];
                verts_rvg[(numVertices_rvg * 3) + 1] = vertex[i][1];
                verts_rvg[(numVertices_rvg * 3) + 2] = vertex[i][2];
                
                norms_rvg[numVertices_rvg * 3] = normal[i][0];
                norms_rvg[(numVertices_rvg * 3) + 1] = normal[i][1];
                norms_rvg[(numVertices_rvg * 3) + 2] = normal[i][2];
                
                texCoords_rvg[numVertices_rvg * 2] = texture[i][0];
                texCoords_rvg[(numVertices_rvg * 2) + 1] = texture[i][1];
                
                elements_rvg[numElements_rvg] = numVertices_rvg; //adding the index to the end of the list of elements_rvg/elements_rvg
                numElements_rvg++; //incrementing the 'end' of the list
                numVertices_rvg++; //incrementing coun of vertices
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

+(Class)layerClass
{
    //Code
    return([CAEAGLLayer class]);
}

-(void)layoutSubviews
{
    //Code
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];
     
    GLint width;
    GLint height;
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is not Complete in layoutsubview!!!\n");
    }
    
    if (height < 0)
    {
        height = 1;
    }
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    
    [self drawView];
}

-(void)pushMatrixWithMat4:(mat4)matrix4by4
{
        //Code
        if (top_rvg >= MAX_rvg - 1)
        {
            printf("Overflow Condition");
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
            printf("Underflow Condition");
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
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use Shader Program Object
        glUseProgram(gShaderProgramObject_rvg);

        //OpenGL Drawing Code
        mat4 modelViewProjectionMatrix;
        mat4 projectionMatrix;
        mat4 translateMatrix;
        mat4 SrotateMatrix;
        mat4 ErotateMatrix;
        mat4 scaleMatrix;

        //Set modelViewMatrix and projectionMatrix to Identity
        modelViewProjectionMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = mat4::identity();
        scaleMatrix = mat4::identity();
        SrotateMatrix = mat4::identity();
        ErotateMatrix = mat4::identity();

        translateMatrix = vmath::translate(0.0f, 0.0f, -25.0f);
        SrotateMatrix = vmath::rotate((GLfloat)Shoulder_rvg, 0.0f, 0.0f, 1.0f);
        modelViewProjectionMatrix = translateMatrix * SrotateMatrix;
        
        [self pushMatrixWithMat4:modelViewProjectionMatrix];
            translateMatrix = vmath::translate(3.6f, 0.0f, 0.0f);
            modelViewProjectionMatrix = modelViewProjectionMatrix * translateMatrix;

            [self pushMatrixWithMat4:modelViewProjectionMatrix];
                scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
                modelViewProjectionMatrix = modelViewProjectionMatrix * scaleMatrix;
                modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT
                glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

                glVertexAttrib3f(RVG_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);

                //Play Cassette
                glBindVertexArray(vao_sphere_rvg);

                //Draw Object
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
                glDrawElements(GL_TRIANGLES, numElements_rvg, GL_UNSIGNED_SHORT, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                //Stop Playing Cassette
                glBindVertexArray(0);
            modelViewProjectionMatrix = [self popMatrix];


            translateMatrix = vmath::translate(4.0f, 0.0f, 0.0f);
            modelViewProjectionMatrix = modelViewProjectionMatrix * translateMatrix;
            ErotateMatrix = vmath::rotate((GLfloat)Elbow_rvg, 0.0f, 0.0f, 1.0f);
            modelViewProjectionMatrix = modelViewProjectionMatrix * ErotateMatrix;
            translateMatrix = vmath::translate(4.0f, 0.0f, 0.0f);
            modelViewProjectionMatrix = modelViewProjectionMatrix * translateMatrix;

            [self pushMatrixWithMat4:modelViewProjectionMatrix];
                scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
                modelViewProjectionMatrix = modelViewProjectionMatrix * scaleMatrix;
                modelViewProjectionMatrix = perspectiveProjectionMatrix_rvg * modelViewProjectionMatrix;    //ORDER IS IMPORTANT
                glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, GL_FALSE, modelViewProjectionMatrix);

                glVertexAttrib3f(RVG_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);

                //Play Cassette
                glBindVertexArray(vao_sphere_rvg);

                //Draw Object
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);
                glDrawElements(GL_TRIANGLES, numElements_rvg, GL_UNSIGNED_SHORT, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                //Stop Playing Cassette
                glBindVertexArray(0);
            modelViewProjectionMatrix = [self popMatrix];
        modelViewProjectionMatrix = [self popMatrix];

        //Stop using Shader Program Object
        glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
}

-(void)startAnimation
{
    //Code
    if (isAnimating == NO)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating = YES;
    }
}

-(void)stopAnimation
{
    //Code
    if (isAnimating == YES)
    {
        [displayLink invalidate];
        displayLink = nil;
        
        isAnimating = NO;
    }
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    //Code
    if (bOnShoulder == true)
    {
        Shoulder_rvg = (Shoulder_rvg + 15) % 360;
    }
    else
    {
        Elbow_rvg = (Elbow_rvg + 15) % 360;
    }
}

-(void)onDoubletap:(UITapGestureRecognizer *)gr
{
    //Code
    if (bOnShoulder == true)
    {
        printf("Elbow Rotation!\n");
        bOnShoulder = false;
        bOnElbow = true;
    }
    else
    {
        printf("Shoulder Rotation!\n");
        bOnElbow = false;
        bOnShoulder = true;
    }
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    //Code
    [self unintialize];
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    //Code
    if (bOnShoulder == true)
    {
        Shoulder_rvg = (Shoulder_rvg - 15) % 360;
    }
    else
    {
        Elbow_rvg = (Elbow_rvg - 15) % 360;
    }
}

-(void)unintialize
{
    //Code
    if (depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    
    if (colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
    if (defaultFramebuffer)
    {
        glDeleteRenderbuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if (eaglContext)
    {
        if ([EAGLContext currentContext] == eaglContext)
        {
            [EAGLContext setCurrentContext:nil];
            [eaglContext release];
            eaglContext = nil;
        }
    }
}

-(void)dealloc
{
    //Code
    [self unintialize];
    [super dealloc];

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

}

@end


