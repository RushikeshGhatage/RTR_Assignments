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

    GLuint vao_plain_rvg;                    //Vertex Array Objects
    GLuint vbo_position_plain_rvg;           //Vertex Buffer Objects
    GLuint vbo_texture_plain_rvg;            //Vertex Buffer Objects for Colors

    GLuint mvpMatrixUniform_rvg;

    GLuint Smiley_Texture;
    GLuint textureSamplerUniform;
    GLuint defaultColorUniform;

    GLfloat CAngle_rvg;

    unsigned int PressedDigit;

    mat4 perspectiveProjectionMatrix_rvg;
}

-(id)initWithFrame:(CGRect)frame
{
    //Code
    PressedDigit = 0;
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
                    glGetShaderInfoLog(gFragmentShaderObject_rvg, infoLogLength, &written, szBuffer);
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
        textureSamplerUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_texture_sampler");
        defaultColorUniform = glGetUniformLocation(gShaderProgramObject_rvg, "u_whiteColor");

        //Vertices Array Declaration
        const GLfloat plainVertices_rvg[] =
        {
            1.0f, 1.0f, 0.0f,       //Front
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };

        //Create Cassette to use Buffer Array in Display()
        glGenVertexArrays(1, &vao_plain_rvg);
        glBindVertexArray(vao_plain_rvg);

                ///////////////////////////// For Vertices /////////////////////////////
        
                //Generate Buffer Array
                glGenBuffers(1, &vbo_position_plain_rvg);
     
                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plain_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, sizeof(plainVertices_rvg), plainVertices_rvg, GL_STATIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_POSITION);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);
        
                /////////////////////////////// FOR TEXTURE ///////////////////////////////
                //Generate Buffer Array
                glGenBuffers(1, &vbo_texture_plain_rvg);

                //Bind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_plain_rvg);

                //Fill Data into Buffer Array
                glBufferData(GL_ARRAY_BUFFER, (4 * 2 * sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);

                //Tell him How to Read Data from Buffer Array
                glVertexAttribPointer(RVG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

                //Enable RVG_ATTRIBUTE_POSITION Point
                glEnableVertexAttribArray(RVG_ATTRIBUTE_TEXCOORD);

                //Unbind Buffer Array
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Stop Creating Cassette
        glBindVertexArray(0);

        //SetClearDepth
        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //Set Texture
        glEnable(GL_TEXTURE_2D);
        
        Smiley_Texture = [self loadGLTexture:@"Smiley" withExtension:@"bmp"];
        if (Smiley_Texture == 0)
        {
            printf("Smiley Texture Failed!\n");
            [self unintialize];
            [self release];
        }
        
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

-(GLuint)loadGLTexture:(NSString *) imageFileName withExtension:(NSString *)extension
{
        //Code
        GLuint textureId;
    
        NSString *imageFilenameWithPath = [[NSBundle mainBundle]pathForResource:imageFileName ofType:extension];
        UIImage *bmpImage = [[UIImage alloc]initWithContentsOfFile:imageFilenameWithPath];

        //Error Changing
        if (!bmpImage)
        {
            printf("NSImage Conversion of file is Failed!\n");
            return(0);
        }
        
        //Get cgImage representation from NSImage
        CGImageRef cgImage = [bmpImage CGImage];
        
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
    //Local Variable Declaration
    static GLfloat TexCoord[8];

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
    glBindVertexArray(vao_plain_rvg);

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
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_plain_rvg);

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

    //Stop using Shader Program Object
    glUseProgram(0);
        
    [self updateView];
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
}

-(void)updateView
{
    //Code

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
    if (PressedDigit >= 5)
    {
        PressedDigit = 0;
    }
    PressedDigit++;
    printf("Pressed Digit = %d\n",PressedDigit);
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
        if (vao_plain_rvg)
        {
            glDeleteVertexArrays(1, &vao_plain_rvg);
            vao_plain_rvg = 0;
        }

        if (vbo_position_plain_rvg)
        {
            glDeleteBuffers(1, &vbo_position_plain_rvg);
            vbo_position_plain_rvg = 0;
        }

        if (vbo_texture_plain_rvg)
        {
            glDeleteBuffers(1, &vbo_texture_plain_rvg);
            vbo_texture_plain_rvg = 0;
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

}

@end











