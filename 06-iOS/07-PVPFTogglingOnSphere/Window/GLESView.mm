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

        ///////////// For Per Vertex /////////////
        GLuint gVertexShaderObject_pv_rvg;
        GLuint gFragmentShaderObject_pv_rvg;
        GLuint gShaderProgramObject_pv_rvg;

        GLuint LaUniform_pv_rvg, LdUniform_pv_rvg, LsUniform_pv_rvg;
        GLuint KaUniform_pv_rvg, KdUniform_pv_rvg, KsUniform_pv_rvg;
        GLint shininessUniform_pv_rvg;
        GLuint lightPositionUniform_pv_rvg;

        GLuint modelMatrixUniform_pv_rvg;
        GLuint viewMatrixUniform_pv_rvg;
        GLuint perspectiveProjectionUniform_pv_rvg;
        GLuint LKeyPressedUniform_pv_rvg;

        ///////////// For Per Fragment /////////////
        GLuint gVertexShaderObject_pf_rvg;
        GLuint gFragmentShaderObject_pf_rvg;
        GLuint gShaderProgramObject_pf_rvg;

        GLuint LaUniform_pf_rvg, LdUniform_pf_rvg, LsUniform_pf_rvg;
        GLuint KaUniform_pf_rvg, KdUniform_pf_rvg, KsUniform_pf_rvg;
        GLint shininessUniform_pf_rvg;
        GLuint lightPositionUniform_pf_rvg;

        GLuint modelMatrixUniform_pf_rvg;
        GLuint viewMatrixUniform_pf_rvg;
        GLuint perspectiveProjectionUniform_pf_rvg;
        GLuint LKeyPressedUniform_pf_rvg;

        ///////////// Common /////////////
        GLuint vao_sphere_rvg;                    //Vertex Array Objects
        GLuint vbo_sphere_position_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_normal_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_texture_rvg;           //Vertex Buffer Objects
        GLuint vbo_sphere_element_rvg;           //Vertex Buffer Objects
      
        int numElements_rvg;
        int numVertices_rvg;
        int maxElements_rvg;

        float *verts_rvg;
        float *norms_rvg;
        float *texCoords_rvg;
        short *elements_rvg;

        bool bPerVertexLight_rvg;
        bool bPerFragmentLight_rvg;

        bool bLight_rvg;

        GLfloat lightAmbient_rvg[4];
        GLfloat lightDiffuse_rvg[4];
        GLfloat lightSpecular_rvg[4];
        GLfloat lightPosition_rvg[4];

        GLfloat materialAmbient_rvg[4];
        GLfloat materialDiffuse_rvg[4];
        GLfloat materialSpecular_rvg[4];
        GLfloat materialShininess_rvg;

    mat4 perspectiveProjectionMatrix_rvg;
}

-(id)initWithFrame:(CGRect)frame
{
        //Variable Initialization
        lightAmbient_rvg[0] = 0.0f;
        lightAmbient_rvg[1] = 0.0f;
        lightAmbient_rvg[2] = 0.0f;
        lightAmbient_rvg[3] = 1.0f;

        lightDiffuse_rvg[0] = 1.0f;
        lightDiffuse_rvg[1] = 1.0f;
        lightDiffuse_rvg[2] = 1.0f;
        lightDiffuse_rvg[3] = 1.0f;

        lightSpecular_rvg[0] = 1.0f;
        lightSpecular_rvg[1] = 1.0f;
        lightSpecular_rvg[2] = 1.0f;
        lightSpecular_rvg[3] = 1.0f;

        lightPosition_rvg[0] = 100.0f;
        lightPosition_rvg[1] = 100.0f;
        lightPosition_rvg[2] = 100.0f;
        lightPosition_rvg[3] = 1.0f;

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

        materialShininess_rvg = 50.0f;

        bPerVertexLight_rvg = true;
        bPerFragmentLight_rvg = false;

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
        
        ///////////////////////////////////////// PER VERTEX /////////////////////////////////////////
        //Shader Code
        // ****************** Vertex Shader ******************
        //Create Shader
        gVertexShaderObject_pv_rvg = glCreateShader(GL_VERTEX_SHADER);

        //Provide Source Code to Shader
        const GLchar *VertexShaderCode_pv_rvg =
                                        "#version 300 es" \
                                        "\n" \
                                         "in vec4 vPosition_pv;" \
                                        "in vec3 vNormal_pv;" \
                                        
                                        "uniform vec3 u_la_pv;" \
                                        "uniform vec3 u_ld_pv;" \
                                        "uniform vec3 u_ls_pv;" \
                                        "uniform vec4 u_light_position_pv;" \
                                        
                                        "uniform vec3 u_ka_pv;" \
                                        "uniform vec3 u_kd_pv;" \
                                        "uniform vec3 u_ks_pv;" \
                                        "uniform float u_materialShininess_pv;" \

                                        "uniform mat4 u_modelMatrix_pv;" \
                                        "uniform mat4 u_viewMatrix_pv;" \
                                        "uniform mat4 u_projectionMatrix_pv;" \

                                        "uniform mediump int u_lKeyPressed_pv;" \

                                        "out vec3 fong_ads_light_pv;" \

                                        "void main(void)" \
                                        "{"    \
                                            "if(u_lKeyPressed_pv == 1)" \
                                            "{" \
                                                "vec4 eye_coordinates = (u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv);" \
                                                "vec3 transformed_normal_pv = normalize(mat3(u_viewMatrix_pv * u_modelMatrix_pv) * vNormal_pv);" \
                                                "vec3 light_direction_pv = normalize(vec3(u_light_position_pv - eye_coordinates));" \
                                                "vec3 reflection_vector = reflect(-light_direction_pv, transformed_normal_pv);" \
                                                "vec3 view_vector_pv = normalize(-eye_coordinates.xyz);" \
                                                "vec3 ambient = u_la_pv * u_ka_pv;" \
                                                "vec3 diffuse = u_ld_pv * u_kd_pv * max(dot(light_direction_pv, transformed_normal_pv), 0.0);" \
                                                "vec3 specular = u_ls_pv * u_ks_pv * pow(max(dot(reflection_vector, view_vector_pv), 0.0), u_materialShininess_pv);" \
                                                "fong_ads_light_pv = ambient + diffuse + specular;" \
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
        glCompileShader(gVertexShaderObject_pv_rvg);

        //Shader Compilation Error Checking
        GLint infoLogLength = 0;
        GLint shaderCompilationStatus = 0;
        GLchar *szBuffer = NULL;

        glGetShaderiv(gVertexShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
        if (shaderCompilationStatus == GL_FALSE)
        {
            glGetShaderiv(gVertexShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject_pv_rvg, infoLogLength, &written, szBuffer);
                    printf("PER VERTEX : Vertex Shader Compilation Log : %s\n", szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER VERTEX : Vertex Shader Compilation is done Successfully.\n");
        }

        // ****************** Fragment Shader ******************
        //Create Shader
        gFragmentShaderObject_pv_rvg = glCreateShader(GL_FRAGMENT_SHADER);

        //Provide Source Code to Shader
        const GLchar *FragmentShaderCode_pv_rvg =
                                        "#version 300 es" \
                                        "\n" \
                                        "precision highp float;" \
                                        "in vec3 fong_ads_light_pv;" \
                                        "out vec4 fragColor_pv;" \
                                        "void main(void)" \
                                        "{"    \
                                            "fragColor_pv = vec4(fong_ads_light_pv, 1.0);" \
                                        "}";

        //Feed to Shader Object
        glShaderSource(gFragmentShaderObject_pv_rvg, 1, (const GLchar **) &FragmentShaderCode_pv_rvg, NULL);

        //Compile Shader
        glCompileShader(gFragmentShaderObject_pv_rvg);

        //Shader Compilation Error Checking
        infoLogLength = 0;
        shaderCompilationStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
        if (shaderCompilationStatus == GL_FALSE)
        {
            glGetShaderiv(gFragmentShaderObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gFragmentShaderObject_pv_rvg, infoLogLength, &written, szBuffer);
                    printf("PER VERTEX : Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER VERTEX : Fragment Shader Compilation is done Successfully.\n");
        }

        //Shader Linking Code
        //1)Create the Shader Program which capable of linking Shader.
        gShaderProgramObject_pv_rvg = glCreateProgram();

        //2)Attach whichever Shaders you have to this Shader Program Object.
        glAttachShader(gShaderProgramObject_pv_rvg, gVertexShaderObject_pv_rvg);        //Vertex Shader
        glAttachShader(gShaderProgramObject_pv_rvg, gFragmentShaderObject_pv_rvg);    //Fragment Shader

        //3)(Pre-Linking) Bind with Shader Program Object with Attributes
        glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_POSITION, "vPosition_pv");
        glBindAttribLocation(gShaderProgramObject_pv_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

        //4)Then, Link all those attached Shader at once.
        glLinkProgram(gShaderProgramObject_pv_rvg);

        //Shader Linkage Error Checking
        infoLogLength = 0;
        GLint shaderProgramLinkStatus = 0;
        szBuffer = NULL;

        glGetProgramiv(gShaderProgramObject_pv_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus);
        if (shaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(gShaderProgramObject_pv_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject_pv_rvg, infoLogLength, &written, szBuffer);
                    printf("PER VERTEX : Shader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER VERTEX : Shader Program Object Compilation is done Successfully.\n");
        }

        //Get MVP Uniform Location
        LaUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_la_pv");
        LdUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ld_pv");
        LsUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ls_pv");
        lightPositionUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_light_position_pv");

        KaUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ka_pv");
        KdUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_kd_pv");
        KsUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ks_pv");
        shininessUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_materialShininess_pv");

        LKeyPressedUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_lKeyPressed_pv");

        modelMatrixUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_modelMatrix_pv");
        viewMatrixUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_viewMatrix_pv");
        perspectiveProjectionUniform_pv_rvg = glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_projectionMatrix_pv");

        ///////////////////////////////////////// PER FRAGMENT /////////////////////////////////////////
        //Shader Code
        // ****************** Vertex Shader ******************
        //Create Shader
        gVertexShaderObject_pf_rvg = glCreateShader(GL_VERTEX_SHADER);

        //Provide Source Code to Shader
        const GLchar *VertexShaderCode_pf_rvg =
                                        "#version 300 es" \
                                        "\n" \
                                        "in vec4 vPosition_pf;" \
                                        "in vec3 vNormal_pf;" \

                                        "out vec3 transformed_normal_pf;" \
                                        "out vec3 light_direction_pf;" \
                                        "out vec3 view_vector_pf;" \
                                        
                                        "uniform vec4 u_light_position_pf;" \
                                        "uniform mediump int u_lKeyPressed_pf;" \

                                        "uniform mat4 u_modelMatrix_pf;" \
                                        "uniform mat4 u_viewMatrix_pf;" \
                                        "uniform mat4 u_projectionMatrix_pf;" \

                                        "void main(void)" \
                                        "{" \
                                            "if(u_lKeyPressed_pf == 1)" \
                                            "{" \
                                                "vec4 eye_coordinates = (u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf);" \
                                                "transformed_normal_pf = mat3(u_viewMatrix_pf * u_modelMatrix_pf) * vNormal_pf;" \
                                                "light_direction_pf = vec3(u_light_position_pf - eye_coordinates);" \
                                                "view_vector_pf = (-eye_coordinates.xyz);" \
                                            "}" \
                                            "gl_Position = u_projectionMatrix_pf * u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf;" \
                                        "}";

        //Feed to Shader Object
        glShaderSource(gVertexShaderObject_pf_rvg, 1, (const GLchar **) &VertexShaderCode_pf_rvg, NULL);

        //Compile Shader
        glCompileShader(gVertexShaderObject_pf_rvg);

        //Shader Compilation Error Checking
        infoLogLength = 0;
        shaderCompilationStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(gVertexShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
        if (shaderCompilationStatus == GL_FALSE)
        {
            glGetShaderiv(gVertexShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject_pf_rvg, infoLogLength, &written, szBuffer);
                    printf("PER FRAGMENT : Vertex Shader Compilation Log : %s\n", szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER FRAGMENT : Vertex Shader Compilation is done Successfully.\n");
        }

        // ****************** Fragment Shader ******************
        //Create Shader
        gFragmentShaderObject_pf_rvg = glCreateShader(GL_FRAGMENT_SHADER);

        //Provide Source Code to Shader
        const GLchar *FragmentShaderCode_pf_rvg =
                                        "#version 300 es" \
                                        "\n" \
                                        "precision highp float;" \
                                        "out vec4 fragColor_pf;" \

                                        "in vec3 transformed_normal_pf;" \
                                        "in vec3 light_direction_pf;" \
                                        "in vec3 view_vector_pf;" \
                                        
                                        "uniform vec3 u_la_pf;" \
                                        "uniform vec3 u_ld_pf;" \
                                        "uniform vec3 u_ls_pf;" \

                                        "uniform vec3 u_ka_pf;" \
                                        "uniform vec3 u_kd_pf;" \
                                        "uniform vec3 u_ks_pf;" \
                                        "uniform float u_materialShininess_pf;" \
                                        "vec3 fong_ads_light_pf;" \

                                        "uniform mediump int u_lKeyPressed_pf;" \

                                        "void main(void)" \
                                        "{" \

                                        "if(u_lKeyPressed_pf == 1)" \
                                            "{" \
                                                "vec3 normalized_tranformed_normal = normalize(transformed_normal_pf);" \
                                                "vec3 normalized_light_direction = normalize(light_direction_pf);" \
                                                "vec3 normalized_view_vector = normalize(view_vector_pf);"
                                                "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" \
                                                "vec3 ambient = u_la_pf * u_ka_pf;" \
                                                "vec3 diffuse = u_ld_pf * u_kd_pf * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" \
                                                "vec3 specular = u_ls_pf * u_ks_pf * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess_pf);" \
                                                "fong_ads_light_pf = ambient + diffuse + specular;" \
                                            "}" \
                                            "else" \
                                            "{" \
                                                "fong_ads_light_pf = vec3(1.0, 1.0, 1.0);" \
                                            "}" \
                                            "fragColor_pf = vec4(fong_ads_light_pf, 1.0);" \
                                        "}";

        //Feed to Shader Object
        glShaderSource(gFragmentShaderObject_pf_rvg, 1, (const GLchar **) &FragmentShaderCode_pf_rvg, NULL);

        //Compile Shader
        glCompileShader(gFragmentShaderObject_pf_rvg);

        //Shader Compilation Error Checking
        infoLogLength = 0;
        shaderCompilationStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_COMPILE_STATUS, &shaderCompilationStatus);
        if (shaderCompilationStatus == GL_FALSE)
        {
            glGetShaderiv(gFragmentShaderObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gFragmentShaderObject_pf_rvg, infoLogLength, &written, szBuffer);
                    printf("PER FRAGMENT : Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER FRAGMENT : Fragment Shader Compilation is done Successfully.\n");
        }

        //Shader Linking Code
        //1)Create the Shader Program which capable of linking Shader.
        gShaderProgramObject_pf_rvg = glCreateProgram();

        //2)Attach whichever Shaders you have to this Shader Program Object.
        glAttachShader(gShaderProgramObject_pf_rvg, gVertexShaderObject_pf_rvg);        //Vertex Shader
        glAttachShader(gShaderProgramObject_pf_rvg, gFragmentShaderObject_pf_rvg);    //Fragment Shader

        //3)(Pre-Linking) Bind with Shader Program Object with Attributes
        glBindAttribLocation(gShaderProgramObject_pf_rvg, RVG_ATTRIBUTE_POSITION, "vPosition_pf");
        glBindAttribLocation(gShaderProgramObject_pf_rvg, RVG_ATTRIBUTE_NORMAL, "vNormal_pf");

        //4)Then, Link all those attached Shader at once.
        glLinkProgram(gShaderProgramObject_pf_rvg);

        //Shader Linkage Error Checking
        infoLogLength = 0;
        shaderProgramLinkStatus = 0;
        szBuffer = NULL;

        glGetProgramiv(gShaderProgramObject_pf_rvg, GL_LINK_STATUS, &shaderProgramLinkStatus);
        if (shaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(gShaderProgramObject_pf_rvg, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0)
            {
                szBuffer = (GLchar *)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject_pf_rvg, infoLogLength, &written, szBuffer);
                    printf("PER FRAGMENT : Shader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self unintialize];
                    [self release];
                    exit(0);
                }
            }
        }
        else
        {
            printf("PER FRAGMENT : Shader Program Object Compilation is done Successfully.\n");
        }

        //Get MVP Uniform Location
        LaUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_la_pf");
        LdUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ld_pf");
        LsUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ls_pf");
        lightPositionUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_light_position_pf");

        KaUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ka_pf");
        KdUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_kd_pf");
        KsUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ks_pf");
        shininessUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_materialShininess_pf");

        LKeyPressedUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_lKeyPressed_pf");

        modelMatrixUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_modelMatrix_pf");
        viewMatrixUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_viewMatrix_pf");
        perspectiveProjectionUniform_pf_rvg = glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_projectionMatrix_pf");

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

-(void)drawView
{
    //Code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use Shader Program Object
        if (bPerVertexLight_rvg == true)
        {
            glUseProgram(gShaderProgramObject_pv_rvg);

            if (bLight_rvg == true)
            {
                glUniform1i(LKeyPressedUniform_pv_rvg, 1);
                
                glUniform3fv(LaUniform_pv_rvg, 1, lightAmbient_rvg);
                glUniform3fv(LdUniform_pv_rvg, 1, lightDiffuse_rvg);
                glUniform3fv(LsUniform_pv_rvg, 1, lightSpecular_rvg);

                glUniform3fv(KaUniform_pv_rvg, 1, materialAmbient_rvg);
                glUniform3fv(KdUniform_pv_rvg, 1, materialDiffuse_rvg);
                glUniform3fv(KsUniform_pv_rvg, 1, materialSpecular_rvg);

                glUniform1f(shininessUniform_pv_rvg, materialShininess_rvg);
                glUniform4fv(lightPositionUniform_pv_rvg, 1, lightPosition_rvg);
            }
            else
            {
                glUniform1i(LKeyPressedUniform_pv_rvg, 0);
            }
        }
        else
        {
            glUseProgram(gShaderProgramObject_pf_rvg);

            if (bLight_rvg == true)
            {
                glUniform1i(LKeyPressedUniform_pf_rvg, 1);
                
                glUniform3fv(LaUniform_pf_rvg, 1, lightAmbient_rvg);
                glUniform3fv(LdUniform_pf_rvg, 1, lightDiffuse_rvg);
                glUniform3fv(LsUniform_pf_rvg, 1, lightSpecular_rvg);

                glUniform3fv(KaUniform_pf_rvg, 1, materialAmbient_rvg);
                glUniform3fv(KdUniform_pf_rvg, 1, materialDiffuse_rvg);
                glUniform3fv(KsUniform_pf_rvg, 1, materialSpecular_rvg);

                glUniform1f(shininessUniform_pf_rvg, materialShininess_rvg);
                glUniform4fv(lightPositionUniform_pf_rvg, 1, lightPosition_rvg);
            }
            else
            {
                glUniform1i(LKeyPressedUniform_pf_rvg, 0);
            }
        }

        //OpenGL Drawing Code
        mat4 modelMatrix;
        mat4 viewMatrix;
        mat4 projectionMatrix;
        mat4 translateMatrix;

        //Set modelViewMatrix and projectionMatrix to Identity
        modelMatrix = mat4::identity();
        viewMatrix = mat4::identity();
        projectionMatrix = mat4::identity();
        translateMatrix = mat4::identity();

        translateMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
        projectionMatrix = perspectiveProjectionMatrix_rvg;
        modelMatrix = translateMatrix;

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

            //Bind Buffer Array
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);

            glDrawElements(GL_TRIANGLES, numElements_rvg, GL_UNSIGNED_SHORT, 0);

            //Unbind Buffer Array
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Stop Playing Cassette
        glBindVertexArray(0);

        //Stop Using Shader PrpgramObject
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
    if (bLight_rvg == true)
    {
        printf("Light Disabled.\n");
        bLight_rvg = false;
    }
    else
    {
        printf("Light Enabled.\n");
        bLight_rvg = true;
    }
}

-(void)onDoubletap:(UITapGestureRecognizer *)gr
{
    //Code
    if (bPerVertexLight_rvg == true)
    {
        printf("Per-Fragment Lighting Enabled.\n");
        bPerVertexLight_rvg = false;
        bPerFragmentLight_rvg = true;
    }
    else
    {
        printf("Per-Vertex Lighting Enabled.\n");
        bPerFragmentLight_rvg = false;
        bPerVertexLight_rvg = true;
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
        if (gShaderProgramObject_pv_rvg)
        {
            glUseProgram(gShaderProgramObject_pv_rvg);
            
            GLsizei shaderCount;
            GLuint *pShader = NULL;
            glGetProgramiv(gShaderProgramObject_pv_rvg, GL_ATTACHED_SHADERS, &shaderCount);

            pShader = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

            glGetAttachedShaders(gShaderProgramObject_pv_rvg, shaderCount, &shaderCount, pShader);

            for (GLsizei i = 0; i < shaderCount; i++)
            {
                glDetachShader(gShaderProgramObject_pv_rvg, pShader[i]);
                glDeleteShader(pShader[i]);
                pShader[i] = 0;
            }
            free(pShader);

            glDeleteProgram(gShaderProgramObject_pv_rvg);
            gShaderProgramObject_pv_rvg = 0;
            glUseProgram(0);
        }

}

@end




