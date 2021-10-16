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

    GLuint vao_pyramid_rvg;                //Vertex Array Objects
    GLuint vbo_position_pyramid_rvg;    //Vertex Buffer Objects for Vertex
    GLuint vbo_color_pyramid_rvg;        //Vertex Buffer Objects for Colors

    GLuint vao_cube_rvg;                    //Vertex Array Objects
    GLuint vbo_position_cube_rvg;           //Vertex Buffer Objects
    GLuint vbo_color_cube_rvg;            //Vertex Buffer Objects for Colors

    GLuint mvpMatrixUniform_rvg;

    GLfloat CAngle_rvg;
    GLfloat PAngle_rvg;

    mat4 perspectiveProjectionMatrix_rvg;
}

-(id)initWithFrame:(CGRect)frame
{
    //Code
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
        glGenVertexArrays(1, &vao_pyramid_rvg);
        glBindVertexArray(vao_pyramid_rvg);
                /////////////////////////////// FOR VERTEX ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_position_pyramid_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid_rvg);

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
                 glGenBuffers(1, &vbo_color_pyramid_rvg);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid_rvg);

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


        //Create Cassette to use Buffer Array in Display()
        glGenVertexArrays(1, &vao_cube_rvg);
        glBindVertexArray(vao_cube_rvg);

                ///////////////////////////// For Vertices /////////////////////////////
        
                //Generate Buffer Array
                glGenBuffers(1, &vbo_position_cube_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube_rvg);

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
                 glGenBuffers(1, &vbo_color_cube_rvg);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_COLOR);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Stop Creating Cassette
        glBindVertexArray(0);

        //SetClearDepth
        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
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
        glBindVertexArray(vao_pyramid_rvg);

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
        glBindVertexArray(vao_cube_rvg);

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
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
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
    
}

-(void)onDoubletap:(UITapGestureRecognizer *)gr
{
    //Code
    
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
        if (vao_pyramid_rvg)
        {
            glDeleteVertexArrays(1, &vao_pyramid_rvg);
            vao_pyramid_rvg = 0;
        }

        if (vbo_position_pyramid_rvg)
        {
            glDeleteBuffers(1, &vbo_position_pyramid_rvg);
            vbo_position_pyramid_rvg = 0;
        }

        if (vbo_color_pyramid_rvg)
        {
            glDeleteBuffers(1, &vbo_color_pyramid_rvg);
            vbo_color_pyramid_rvg = 0;
        }

        if (vao_cube_rvg)
        {
            glDeleteVertexArrays(1, &vao_cube_rvg);
            vao_cube_rvg = 0;
        }

        if (vbo_position_cube_rvg)
        {
            glDeleteBuffers(1, &vbo_position_cube_rvg);
            vbo_position_cube_rvg = 0;
        }

        if (vbo_color_cube_rvg)
        {
            glDeleteBuffers(1, &vbo_color_cube_rvg);
            vbo_color_cube_rvg = 0;
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






