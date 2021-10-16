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

        [window setTitle: @"RVG : macOS 24 Spheres"];
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

        GLuint LaUniform_rvg, LdUniform_rvg, LsUniform_rvg;
        GLuint lightPositionUniform_rvg;

        GLuint modelMatrixUniform_rvg;
        GLuint viewMatrixUniform_rvg;
        GLuint perspectiveProjectionUniform_rvg;
        GLuint LKeyPressedUniform_rvg;

        struct Material_rvg
        {
            vec4 materialAmbient_rvg;
            vec4 materialDiffuse_rvg;
            vec4 materialSpecular_rvg;
            float materialShininess_rvg;
        };
        struct Material_rvg material_rvg[24];

        struct Uniform_rvg
        {
            GLuint KaUniform_rvg;
            GLuint KdUniform_rvg;
            GLuint KsUniform_rvg;
            GLuint shininessUniform_rvg;
        };
        struct Uniform_rvg uniform_rvg[24];

        GLuint gWidth_rvg, gHeight_rvg;

        bool bLight_rvg;
        bool XKeyPressed_rvg;
        bool YKeyPressed_rvg;
        bool ZKeyPressed_rvg;

        GLfloat lightAngleX_rvg;
        GLfloat lightAngleY_rvg;
        GLfloat lightAngleZ_rvg;

        GLfloat lightAmbient_rvg[4];
        GLfloat lightDiffuse_rvg[4];
        GLfloat lightSpecular_rvg[4];
        GLfloat lightPosition_rvg[4];

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
        XKeyPressed_rvg = false;
        YKeyPressed_rvg = false;
        ZKeyPressed_rvg = false;

        lightAngleX_rvg = 0.0f;
        lightAngleY_rvg = 0.0f;
        lightAngleZ_rvg = 0.0f;

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

        lightPosition_rvg[0] = 0.0f;
        lightPosition_rvg[1] = 0.0f;
        lightPosition_rvg[2] = 0.0f;
        lightPosition_rvg[3] = 1.0f;

        ///////////// Material Declaration /////////////

        material_rvg[0].materialAmbient_rvg = vec4(0.215f, 0.1745f, 0.0215f, 1.0f);                 // (1)
        material_rvg[0].materialDiffuse_rvg = vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
        material_rvg[0].materialSpecular_rvg = vec4(0.633f, 0.727811f, 0.633f, 1.0f);
        material_rvg[0].materialShininess_rvg = 0.6f * 128.0f;

        material_rvg[1].materialAmbient_rvg = vec4(0.135f, 0.2225f, 0.1575f, 1.0f);                 // (2)
        material_rvg[1].materialDiffuse_rvg = vec4(0.54f, 0.89f, 0.63f, 1.0f);
        material_rvg[1].materialSpecular_rvg = vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
        material_rvg[1].materialShininess_rvg = 0.1f * 128.0f;

        material_rvg[2].materialAmbient_rvg = vec4(0.05375f, 0.05f, 0.0625f, 1.0f);                 // (3)
        material_rvg[2].materialDiffuse_rvg = vec4(0.18275f, 0.17f, 0.22525f, 1.0f);
        material_rvg[2].materialSpecular_rvg = vec4(0.332741f, 0.338634f, 0.346435f, 1.0f);
        material_rvg[2].materialShininess_rvg = 0.3f * 128.0f;

        material_rvg[3].materialAmbient_rvg = vec4(0.25f, 0.20725f, 0.20725f, 1.0f);                // (4)
        material_rvg[3].materialDiffuse_rvg = vec4(1.0f, 0.829f, 0.829f, 1.0f);
        material_rvg[3].materialSpecular_rvg = vec4(0.296648f, 0.296648f, 0.296648f, 1.0f);
        material_rvg[3].materialShininess_rvg = 0.088f *128.0f;

        material_rvg[4].materialAmbient_rvg = vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);              // (5)
        material_rvg[4].materialDiffuse_rvg = vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
        material_rvg[4].materialSpecular_rvg = vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
        material_rvg[4].materialShininess_rvg = 0.6f * 128.0f;

        material_rvg[5].materialAmbient_rvg = vec4(1.0f, 0.18725f, 0.1745f, 1.0f);                  // (6)
        material_rvg[5].materialDiffuse_rvg = vec4(0.396f, 0.74191f, 0.69102f, 1.0f);
        material_rvg[5].materialSpecular_rvg = vec4(0.297254f, 0.30829f, 0.306678f, 1.0f);
        material_rvg[5].materialShininess_rvg = 0.1f * 128.0f;

        material_rvg[6].materialAmbient_rvg = vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);          // (7)
        material_rvg[6].materialDiffuse_rvg = vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
        material_rvg[6].materialSpecular_rvg = vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
        material_rvg[6].materialShininess_rvg = 0.21794872f * 128.0f;

        material_rvg[7].materialAmbient_rvg = vec4(0.2125f, 0.1275f, 0.054f, 1.0f);                 // (8)
        material_rvg[7].materialDiffuse_rvg = vec4(0.714f, 0.4284f, 0.18144f, 1.0f);
        material_rvg[7].materialSpecular_rvg = vec4(0.393548f, 0.271906f, 0.163721f, 1.0f);
        material_rvg[7].materialShininess_rvg = 0.2f * 128.0f;

        material_rvg[8].materialAmbient_rvg = vec4(0.25f, 0.25f, 0.25f, 1.0f);                      // (9)
        material_rvg[8].materialDiffuse_rvg = vec4(0.4f, 0.4f, 0.4f, 1.0f);
        material_rvg[8].materialSpecular_rvg = vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
        material_rvg[8].materialShininess_rvg = 0.6f * 128.0f;

        material_rvg[9].materialAmbient_rvg = vec4(0.19125f, 0.735f, 0.0225f, 1.0f);                // (10)
        material_rvg[9].materialDiffuse_rvg = vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
        material_rvg[9].materialSpecular_rvg = vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
        material_rvg[9].materialShininess_rvg = 0.1 * 128.0f;

        material_rvg[10].materialAmbient_rvg = vec4(0.24725f, 0.1795f, 0.0745f, 1.0f);              // (11)
        material_rvg[10].materialDiffuse_rvg = vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
        material_rvg[10].materialSpecular_rvg = vec4(0.658281f, 0.555802f, 0.366065f, 1.0f);
        material_rvg[10].materialShininess_rvg = 0.4f * 128.0f;

        material_rvg[11].materialAmbient_rvg = vec4(0.19225f, 0.19225f, 0.19225f, 1.0f);            // (12)
        material_rvg[11].materialDiffuse_rvg = vec4(0.50754f, 0.50754f, 0.50754f, 1.0f);
        material_rvg[11].materialSpecular_rvg = vec4(0.508273f, 0.508273f, 0.508273f, 1.0f);
        material_rvg[11].materialShininess_rvg = 0.4f * 128.0f;

        material_rvg[12].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);                        // (13)
        material_rvg[12].materialDiffuse_rvg = vec4(0.01f, 0.01f, 0.01f, 1.0f);
        material_rvg[12].materialSpecular_rvg = vec4(0.50f, 0.50f, 0.50f, 1.0f);
        material_rvg[12].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[13].materialAmbient_rvg = vec4(0.0f, 0.1f, 0.06f, 1.0f);                       // (14)
        material_rvg[13].materialDiffuse_rvg = vec4(0.0f, 0.50960392f, 0.50960392f, 1.0f);
        material_rvg[13].materialSpecular_rvg = vec4(0.50196078f, 0.50196078f, 0.50196078f, 1.0f);
        material_rvg[13].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[14].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);                        // (15)
        material_rvg[14].materialDiffuse_rvg = vec4(0.1f, 0.35f, 0.1f, 1.0f);
        material_rvg[14].materialSpecular_rvg = vec4(0.45f, 0.55f, 0.45f, 1.0f);
        material_rvg[14].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[15].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);                        // (16)
        material_rvg[15].materialDiffuse_rvg = vec4(0.5f, 0.0f, 0.0f, 1.0f);
        material_rvg[15].materialSpecular_rvg = vec4(0.7f, 0.6f, 0.6f, 1.0f);
        material_rvg[15].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[16].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);                        // (17)
        material_rvg[16].materialDiffuse_rvg = vec4(0.55f, 0.55f, 0.55f, 1.0f);
        material_rvg[16].materialSpecular_rvg = vec4(0.70f, 0.70f, 0.70f, 1.0f);
        material_rvg[16].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[17].materialAmbient_rvg = vec4(0.0f, 0.0f, 0.0f, 1.0f);                        // (18)
        material_rvg[17].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.0f, 1.0f);
        material_rvg[17].materialSpecular_rvg = vec4(0.60f, 0.60f, 0.50f, 1.0f);
        material_rvg[17].materialShininess_rvg = 0.25f * 128.0f;

        material_rvg[18].materialAmbient_rvg = vec4(0.02f, 0.02f, 0.02f, 1.0f);                     // (19)
        material_rvg[18].materialDiffuse_rvg = vec4(0.01f, 0.01f, 0.01f, 1.0f);
        material_rvg[18].materialSpecular_rvg = vec4(0.40f, 0.40f, 0.40f, 1.0f);
        material_rvg[18].materialShininess_rvg = 0.078125f * 128.0f;

        material_rvg[19].materialAmbient_rvg = vec4(0.0f, 0.05f, 0.05f, 1.0f);                      // (20)
        material_rvg[19].materialDiffuse_rvg = vec4(0.4f, 0.5f, 0.5f, 1.0f);
        material_rvg[19].materialSpecular_rvg = vec4(0.04f, 0.70f, 0.70f, 1.0f);
        material_rvg[19].materialShininess_rvg = 0.078125f * 128.0f;

        material_rvg[20].materialAmbient_rvg = vec4(0.0f, 0.05f, 0.0f, 1.0f);                       // (21)
        material_rvg[20].materialDiffuse_rvg = vec4(0.4f, 0.5f, 0.4f, 1.0f);
        material_rvg[20].materialSpecular_rvg = vec4(0.04f, 0.7f, 0.04f, 1.0f);
        material_rvg[20].materialShininess_rvg = 0.078125f * 128.0f;

        material_rvg[21].materialAmbient_rvg = vec4(0.05f, 0.0f, 0.0f, 1.0f);                       // (22)
        material_rvg[21].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.4f, 1.0f);
        material_rvg[21].materialSpecular_rvg = vec4(0.7f, 0.04f, 0.06f, 1.0f);
        material_rvg[21].materialShininess_rvg = 0.078125f *  128.0f;

        material_rvg[22].materialAmbient_rvg = vec4(0.05f, 0.05f, 0.05f, 1.0f);                     // (23)
        material_rvg[22].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        material_rvg[22].materialSpecular_rvg = vec4(0.7f, 0.7f, 0.7f, 1.0f);
        material_rvg[22].materialShininess_rvg = 0.078125f * 128.0f;

        material_rvg[23].materialAmbient_rvg = vec4(0.05f, 0.05f, 0.0f, 1.0f);                      // (24)
        material_rvg[23].materialDiffuse_rvg = vec4(0.5f, 0.5f, 0.4f, 1.0f);
        material_rvg[23].materialSpecular_rvg = vec4(0.7f, 0.7f, 0.04f, 1.0f);
        material_rvg[23].materialShininess_rvg = 0.078125f * 128.0f;

        ////////////////////////////////////////////////

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
                                        "in vec3 vNormal;" \

                                        "out vec3 transformed_normal;" \
                                        "out vec3 light_direction;" \
                                        "out vec3 view_vector;" \
                                        
                                        "uniform vec4 u_light_position;" \
                                        "uniform int u_lKeyPressed;" \

                                        "uniform mat4 u_modelMatrix;" \
                                        "uniform mat4 u_viewMatrix;" \
                                        "uniform mat4 u_projectionMatrix;" \

                                        "void main(void)" \
                                        "{" \
                                            "if(u_lKeyPressed == 1)" \
                                            "{" \
                                                "vec4 eye_coordinates = (u_viewMatrix * u_modelMatrix * vPosition);" \
                                                "transformed_normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
                                                "light_direction = vec3(u_light_position - eye_coordinates);" \
                                                "view_vector = (-eye_coordinates.xyz);" \
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
                    fprintf(gpFile_rvg, "Vertex Shader Compilation Log : %s_rvg\n", szBuffer);
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
                                        "out vec4 fragColor;" \

                                        "in vec3 transformed_normal;" \
                                        "in vec3 light_direction;" \
                                        "in vec3 view_vector;" \
                                        
                                        "uniform vec3 u_la;" \
                                        "uniform vec3 u_ld;" \
                                        "uniform vec3 u_ls;" \

                                        "uniform vec3 u_ka;" \
                                        "uniform vec3 u_kd;" \
                                        "uniform vec3 u_ks;" \
                                        "uniform float u_materialShininess;" \
                                        "out vec3 fong_ads_light;" \

                                        "uniform int u_lKeyPressed;" \

                                        "void main(void)" \
                                        "{" \

                                        "if(u_lKeyPressed == 1)" \
                                            "{" \
                                                "vec3 normalized_tranformed_normal = normalize(transformed_normal);" \
                                                "vec3 normalized_light_direction = normalize(light_direction);" \
                                                "vec3 normalized_view_vector = normalize(view_vector);"
                                                "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" \
                                                "vec3 ambient = u_la * u_ka;" \
                                                "vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" \
                                                "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess);" \
                                                "fong_ads_light = ambient + diffuse + specular;" \
                                            "}" \
                                            "else" \
                                            "{" \
                                                "fong_ads_light = vec3(1.0, 1.0, 1.0);" \
                                            "}" \
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
                    fprintf(gpFile_rvg, "Fragment Shader Compilation Log : %s_rvg\n", szBuffer);
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
                    fprintf(gpFile_rvg, "Shader Program Link Log : %s_rvg\n", szBuffer);
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
        LaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_la");
        LdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ld");
        LsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ls");
        lightPositionUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position");

        for (int i = 0; i < 24; i++)
        {
            uniform_rvg[i].KaUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ka");
            uniform_rvg[i].KdUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_kd");
            uniform_rvg[i].KsUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_ks");
            uniform_rvg[i].shininessUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_materialShininess");
        }

        LKeyPressedUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_lKeyPressed");

        modelMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_modelMatrix");
        viewMatrixUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_viewMatrix");
        perspectiveProjectionUniform_rvg = glGetUniformLocation(gShaderProgramObject_rvg, "u_projectionMatrix");


        //Vertices Array Declaration
        [self makeSphereWithRadius:2.0f usingSlices:40 andStacks:40];

        //SetClearDepth
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);        //Hidden Surface Removal
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

        bLight_rvg = false;
        
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

        gWidth_rvg = rect.size.width;
        gHeight_rvg = rect.size.height;
        
        //glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
        
        //perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)rect.size.width / (GLfloat)rect.size.height, 0.1f, 100.0f);
        
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
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
        int i = 0;
        int j = 0;

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
            if (XKeyPressed_rvg == true)
            {
                lightPosition_rvg[0] = 0.0f;
                lightPosition_rvg[1] = 28.0f * sin(lightAngleX_rvg);
                lightPosition_rvg[2] = 28.0f * cos(lightAngleX_rvg);
                lightPosition_rvg[3] = 1.0f;
            }

            else if (YKeyPressed_rvg == true)
            {
                lightPosition_rvg[0] = 28.0f * sin(lightAngleY_rvg);
                lightPosition_rvg[1] = 0.0f;
                lightPosition_rvg[2] = 28.0f * cos(lightAngleY_rvg);
                lightPosition_rvg[3] = 1.0f;
            }

            else if (ZKeyPressed_rvg == true)
            {
                lightPosition_rvg[0] = 28.0f * sin(lightAngleZ_rvg);
                lightPosition_rvg[1] = 28.0f * cos(lightAngleZ_rvg);
                lightPosition_rvg[2] = 0.0f;
                lightPosition_rvg[3] = 1.0f;
            }
            
            glUniform1i(LKeyPressedUniform_rvg, 1);
            
            glUniform3fv(LaUniform_rvg, 1, lightAmbient_rvg);
            glUniform3fv(LdUniform_rvg, 1, lightDiffuse_rvg);
            glUniform3fv(LsUniform_rvg, 1, lightSpecular_rvg);


            glUniform4fv(lightPositionUniform_rvg, 1, lightPosition_rvg);
        }
        else
        {
            glUniform1i(LKeyPressedUniform_rvg, 0);
        }

        //OpenGL Drawing Code
        mat4 modelMatrix;
        mat4 viewMatrix;
        mat4 projectionMatrix;
        mat4 translateMatrix;

        int iMaterial_rvg = 0;
        
        for (float i = 0.0f; i < gHeight_rvg; i = i + gHeight_rvg / 4.0f)
        {
            for (float j = 0.0f; j < gWidth_rvg; j = j + gWidth_rvg / 6.0f)
            {
                glUniform3fv(uniform_rvg[iMaterial_rvg].KaUniform_rvg, 1, material_rvg[iMaterial_rvg].materialAmbient_rvg);
                glUniform3fv(uniform_rvg[iMaterial_rvg].KdUniform_rvg, 1, material_rvg[iMaterial_rvg].materialDiffuse_rvg);
                glUniform3fv(uniform_rvg[iMaterial_rvg].KsUniform_rvg, 1, material_rvg[iMaterial_rvg].materialSpecular_rvg);

                glUniform1f(uniform_rvg[iMaterial_rvg].shininessUniform_rvg, material_rvg[iMaterial_rvg].materialShininess_rvg);

                glViewport((GLsizei)j, (GLsizei)i, (GLsizei)gWidth_rvg / 5.5, (GLsizei)gHeight_rvg / 5.0);
                perspectiveProjectionMatrix_rvg = vmath::perspective(45.0f, (GLfloat)gWidth_rvg / (GLfloat)gHeight_rvg, 0.1f, 100.0f);

                //Set modelViewMatrix and projectionMatrix to Identity
                modelMatrix = mat4::identity();
                viewMatrix = mat4::identity();
                projectionMatrix = mat4::identity();
                translateMatrix = mat4::identity();

                translateMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
                projectionMatrix = perspectiveProjectionMatrix_rvg;
                modelMatrix = translateMatrix;
                
                glUniformMatrix4fv(modelMatrixUniform_rvg, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(viewMatrixUniform_rvg, 1, GL_FALSE, viewMatrix);
                glUniformMatrix4fv(perspectiveProjectionUniform_rvg, 1, GL_FALSE, projectionMatrix);

                //Play Cassette
                glBindVertexArray(vao_sphere_rvg);

                    //Bind Buffer Array
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_rvg);

                    glDrawElements(GL_TRIANGLES, numElements_rvg, GL_UNSIGNED_SHORT, 0);

                    //Unbind Buffer Array
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                //Stop Playing Cassette
                glBindVertexArray(0);

                iMaterial_rvg++;
            }
        }

        //Stop Using Shader PrpgramObject
        glUseProgram(0);
        
        [self updateView];
        
        CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    }

    -(void)updateView
    {
        //Code
        if (bLight_rvg == true)
        {
            if (XKeyPressed_rvg == true)
            {
                lightAngleX_rvg += 0.03f;
            }

            if (YKeyPressed_rvg == true)
            {
                lightAngleY_rvg += 0.03f;
            }

            if (ZKeyPressed_rvg == true)
            {
                lightAngleZ_rvg += 0.03f;
            }

            if (lightAngleX_rvg >= 360.0f)
            {
                lightAngleX_rvg = 0.0f;
                lightAngleY_rvg = 0.0f;
                lightAngleZ_rvg = 0.0f;
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

            case 'X':
            case 'x':
                XKeyPressed_rvg = true;
                YKeyPressed_rvg = false;
                ZKeyPressed_rvg = false;
                lightAngleX_rvg = 0.0f;
                break;

            case 'Y':
            case 'y':
                XKeyPressed_rvg = false;
                YKeyPressed_rvg = true;
                ZKeyPressed_rvg = false;
                lightAngleY_rvg = 0.0f;
                break;

            case 'Z':
            case 'z':
                XKeyPressed_rvg = false;
                YKeyPressed_rvg = false;
                ZKeyPressed_rvg = true;
                lightAngleZ_rvg = 0.0f;
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





