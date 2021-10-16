package com.astromedicomp.TwoLightOnMovingPyramid;

import android.content.Context;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;	//For Sphere

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;
import android.opengl.Matrix;

import android.opengl.GLES32;	//OpenGL ES 3.2 (Version)
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;	//For OpenGL ES 1.0
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context_rvg;
	private GestureDetector gestureDetector_rvg;

	private class Light_rvg
	{
		float[] lightAmbient_rvg = new float[4];
		float[] lightDiffuse_rvg = new float[4];
		float[] lightSpecular_rvg = new float[4];
		float[] lightPosition_rvg = new float[4];
	}
	private Light_rvg light_rvg[] = new Light_rvg[2];	
	
	private int gVertexShaderObject_rvg;
	private int gFragmentShaderObject_rvg;
	private int gShaderProgramObject_rvg;
	
	private int[] vao_pyramid_rvg = new int[1];
	private int[] vbo_pyramid_position_rvg = new int[1];
	private int[] vbo_pyramid_normal_rvg = new int[1];

	private int LaUniform0_rvg, LdUniform0_rvg, LsUniform0_rvg;
	private int LaUniform1_rvg, LdUniform1_rvg, LsUniform1_rvg;
	private int lightPositionUniform0_rvg, lightPositionUniform1_rvg;
	
	private int KaUniform_rvg, KdUniform_rvg, KsUniform_rvg;
	private int shininessUniform_rvg;

	private int modelMatrixUniform_rvg;
	private int viewMatrixUniform_rvg;
	private int perspectiveProjectionUniform_rvg;
	private int LKeyPressedUniform_rvg; 

	boolean bLight_rvg;

	private float PAngle_rvg = 0.0f;

	private float[] materialAmbient_rvg = new float[] {0.0f, 0.0f, 0.0f, 1.0f};
	private float[] materialDiffuse_rvg = new float[] {1.0f, 1.0f, 1.0f, 1.0f};
	private float[] materialSpecular_rvg = new float[] {1.0f, 1.0f, 1.0f, 1.0f};
	private float materialShininess_rvg = 50.0f;

	private float perspectiveProjectionMatrix_rvg[] = new float[16];	//4 X 4 Matrix

	public GLESView(Context drawingContext_rvg)
	{
		super(drawingContext_rvg);

		context_rvg = drawingContext_rvg;

		setEGLContextClientVersion(3);	//Major Version
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector_rvg = new GestureDetector(context_rvg, this, null, false);
		gestureDetector_rvg.setOnDoubleTapListener(this);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		String GLESVersion_rvg = gl.glGetString(GL10.GL_VERSION);
		String GLSLVersion_rvg = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);

		System.out.println("RVG: "+GLESVersion_rvg);
		System.out.println("RVG: "+GLSLVersion_rvg);

		Initialize(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		Resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 unused)
	{
		Display();

		Update();
	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		//Code
		int eventAction = event.getAction();
		if (!gestureDetector_rvg.onTouchEvent(event))
		{
			super.onTouchEvent(event);
		}
		return(true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		if (bLight_rvg == true)
		{
			bLight_rvg = false;
		}
		else
		{
			bLight_rvg = true;
		}	
		return(true);
	}

	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}

	@Override
	public void onLongPress(MotionEvent e)
	{

	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		Uninitialize();
		System.exit(0);
		return(true);
	}

	@Override
	public void onShowPress(MotionEvent e)
	{

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}

	private void Initialize(GL10 gl)
	{
        //Code
		light_rvg[0] = new Light_rvg();
		light_rvg[0].lightAmbient_rvg[0] = 0.0f;
		light_rvg[0].lightAmbient_rvg[1] = 0.0f;
		light_rvg[0].lightAmbient_rvg[2] = 0.0f;
		light_rvg[0].lightAmbient_rvg[3] = 1.0f;

		light_rvg[0].lightDiffuse_rvg[0] = 1.0f;
		light_rvg[0].lightDiffuse_rvg[1] = 0.0f;
		light_rvg[0].lightDiffuse_rvg[2] = 0.0f;
		light_rvg[0].lightDiffuse_rvg[3] = 1.0f;

		light_rvg[0].lightSpecular_rvg[0] = 1.0f;
		light_rvg[0].lightSpecular_rvg[1] = 0.0f;
		light_rvg[0].lightSpecular_rvg[2] = 0.0f;
		light_rvg[0].lightSpecular_rvg[3] = 1.0f;

		light_rvg[0].lightPosition_rvg[0] = -2.0f;
		light_rvg[0].lightPosition_rvg[1] = 0.0f;
		light_rvg[0].lightPosition_rvg[2] = 0.0f;
		light_rvg[0].lightPosition_rvg[3] = 1.0f;
	

		light_rvg[1] = new Light_rvg();
		light_rvg[1].lightAmbient_rvg[0] = 0.0f;
		light_rvg[1].lightAmbient_rvg[1] = 0.0f;
		light_rvg[1].lightAmbient_rvg[2] = 0.0f;
		light_rvg[1].lightAmbient_rvg[3] = 1.0f;

		light_rvg[1].lightDiffuse_rvg[0] = 0.0f;
		light_rvg[1].lightDiffuse_rvg[1] = 0.0f;
		light_rvg[1].lightDiffuse_rvg[2] = 1.0f;
		light_rvg[1].lightDiffuse_rvg[3] = 1.0f;

		light_rvg[1].lightSpecular_rvg[0] = 0.0f;
		light_rvg[1].lightSpecular_rvg[1] = 0.0f;
		light_rvg[1].lightSpecular_rvg[2] = 1.0f;
		light_rvg[1].lightSpecular_rvg[3] = 1.0f;

		light_rvg[1].lightPosition_rvg[0] = 2.0f;
		light_rvg[1].lightPosition_rvg[1] = 0.0f;
		light_rvg[1].lightPosition_rvg[2] = 0.0f;
		light_rvg[1].lightPosition_rvg[3] = 1.0f;		

		// ****************** Vertex Shader ******************
		//Create Shader
		gVertexShaderObject_rvg = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		//Provide Source Code to Shader
		final String vertexShaderCode_rvg = String.format(
									"#version 320 es" +
									"\n" +
									"in vec4 vPosition;" +
									"in vec3 vNormal;" +
									"uniform int u_lKeyPressed;" +

									"uniform mat4 u_modelMatrix;" +
									"uniform mat4 u_viewMatrix;" +
									"uniform mat4 u_projectionMatrix;" +

									"uniform vec3 u_ka;" +
									"uniform vec3 u_kd;" +
									"uniform vec3 u_ks;" +
									"uniform float u_materialShininess;" +
									
									"uniform vec3 u_la[2];" +
									"uniform vec3 u_ld[2];" +
									"uniform vec3 u_ls[2];" +
									"uniform vec4 u_light_position[2];" +

									"out vec3 fong_ads_light;" +

									"void main(void)" +
									"{"	+
										"vec3 ambient[2];" +
										"vec3 diffuse[2];" +
										"vec3 specular[2];" +
										"vec3 light_direction[2];" +
										"vec3 reflection_vector[2];" +

										"if(u_lKeyPressed == 1)" +
										"{" +
											"vec4 eye_coordinates = (u_viewMatrix * u_modelMatrix * vPosition);" +
											"vec3 transformed_normal = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" +
											"vec3 view_vector = normalize(-eye_coordinates.xyz);" +

											"for (int i = 0; i < 2; i++)" +
											"{" +
												"ambient[i] = u_la[i] * u_ka;" +
												"light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" +
												"diffuse[i] = u_ld[i] * u_kd * max(dot(light_direction[i], transformed_normal), 0.0);" +
												"reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" +
												"specular[i] = u_ls[i] * u_ks * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess);" +
												"fong_ads_light = fong_ads_light + ambient[i] + diffuse[i] + specular[i];" +
											"}" +
										"}" +
										"else" +
										"{" +
											"fong_ads_light = vec3(1.0, 1.0, 1.0);" +
										"}" +
										"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" +
									"}");

		//Feed to Shader Object
		GLES32.glShaderSource(gVertexShaderObject_rvg, vertexShaderCode_rvg);

		//Compile Shader
		GLES32.glCompileShader(gVertexShaderObject_rvg);

		//Shader Compilation Error Checking
		int[] infoLogLength_rvg = new int[1];
		int[] shaderCompilationStatus_rvg = new int[1];
		String szBuffer_rvg = null;
		GLES32.glGetShaderiv(gVertexShaderObject_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gVertexShaderObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gVertexShaderObject_rvg);
				System.out.println("RVG: Vertex Shader Compilation Log : "+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: Vertex Shader Compilation is done Successfully.\n");
		}

		// ****************** Fragment Shader ******************
		//Create Shader
		gFragmentShaderObject_rvg = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		//Provide Source Code to Shader
		final String fragmentShaderCode_rvg = String.format(
														"#version 320 es" +
														"\n" +
														"precision highp float;" +
														"in vec3 fong_ads_light;" +
														"out vec4 fragColor;" +
														"void main(void)" +
														"{"	+
															"fragColor = vec4(fong_ads_light, 1.0);" +
														"}");

		//Feed to Shader Object
		GLES32.glShaderSource(gFragmentShaderObject_rvg, fragmentShaderCode_rvg);

		//Compile Shader
		GLES32.glCompileShader(gFragmentShaderObject_rvg);	

		//Shader Compilation Error Checking
		infoLogLength_rvg[0] = 0;
		shaderCompilationStatus_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetShaderiv(gFragmentShaderObject_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gFragmentShaderObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gFragmentShaderObject_rvg);
				System.out.println("RVG: Fragment Shader Compilation Log :\n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: Fragment Shader Compilation is done Successfully.\n");
		}

		//Shader Linking Code
		//1)Create the Shader Program which capable of linking Shader.
		gShaderProgramObject_rvg = GLES32.glCreateProgram();

		//2)Attach whichever Shaders you have to this Shader Program Object.
		GLES32.glAttachShader(gShaderProgramObject_rvg, gVertexShaderObject_rvg);		//Vertex Shader
		GLES32.glAttachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);		//Fragment Shader

		//3)(Pre-Linking) Bind with Shader Program Object with Attributes
		GLES32.glBindAttribLocation(gShaderProgramObject_rvg, GLESMacros.RVG_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(gShaderProgramObject_rvg, GLESMacros.RVG_ATTRIBUTE_NORMAL, "vNormal");

		//4)Then, Link all those attached Shader at once.
		GLES32.glLinkProgram(gShaderProgramObject_rvg);

		//Shader Linkage Error Checking
		int[] shaderProgramLinkStatus_rvg = new int[1];
		infoLogLength_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetProgramiv(gShaderProgramObject_rvg, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus_rvg, 0);
		if (shaderProgramLinkStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(gShaderProgramObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetProgramInfoLog(gShaderProgramObject_rvg);
				System.out.println("RVG: Shader Program Link Log : \n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: Program Shader Object Compilation is done Successfully.\n");
		}

		//Get MVP Uniform Location
		LaUniform0_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_la[0]");
		LdUniform0_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[0]");
		LsUniform0_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[0]");
		lightPositionUniform0_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[0]");

		LaUniform1_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_la[1]");
		LdUniform1_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ld[1]");
		LsUniform1_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ls[1]");
		lightPositionUniform1_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_light_position[1]");

		KaUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ka");
		KdUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_kd");
		KsUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_ks");
		shininessUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_materialShininess");

		LKeyPressedUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_lKeyPressed");

		modelMatrixUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_modelMatrix");
		viewMatrixUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_viewMatrix");
		perspectiveProjectionUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_projectionMatrix");

		//Vertices Array Declaration
		final float pyramidVertices_rvg[] = new float[]
		{
			0.0f, 1.0f, 0.00f, 		//Apex
			-1.0f, -1.0f, 1.0f, 	//Front Left Bottom
			1.0f, -1.0f, 1.0f,		//Front Right Bottom
			
			0.0f, 1.0f, 0.0f,		//Apex
			1.0f, -1.0f, -1.0f,		//Back Right Bottom
			-1.0f, -1.0f, -1.0f,	//Back Left Bottom

			0.0f, 1.0f, 0.0f,		//Apex
			-1.0f, -1.0f, -1.0f,	//Left Left Bottom
			-1.0f, -1.0f, 1.0f,		//Left Right Bottom

			0.0f, 1.0f, 0.0f,		//Apex
			1.0f, -1.0f, 1.0f,		//Right Left Bottom
			1.0f, -1.0f, -1.0f  	//Right Right Bottom
		};

		final float pyramidNormals_rvg[] = new float[]
		{
			0.0f, 0.447214f, 0.894427f,		//Front
			0.0f, 0.447214f, 0.894427f,
			0.0f, 0.447214f, 0.894427f,

			0.0f, 0.447214f, -0.894427f,	//Back
			0.0f, 0.447214f, -0.894427f,
			0.0f, 0.447214f, -0.894427f,

			-0.894427f, 0.447214f, 0.0f,	//Left
			-0.894427f, 0.447214f, 0.0f,
			-0.894427f, 0.447214f, 0.0f,

			0.894427f, 0.447214f, 0.0f,		//Right
			0.894427f, 0.447214f, 0.0f,
			0.894427f, 0.447214f, 0.0f
		};				


		//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao_pyramid_rvg, 0);
		GLES32.glBindVertexArray(vao_pyramid_rvg[0]);
				/////////////////////////// For POSITION ///////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_pyramid_position_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pyramid_position_rvg[0]);

				//Fill Data into Buffer Array
				ByteBuffer byteBuffer_rvg = ByteBuffer.allocateDirect(pyramidVertices_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(pyramidVertices_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_POSITION);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				/////////////////////////// For NORMAL ///////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_pyramid_normal_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pyramid_normal_rvg[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(pyramidNormals_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(pyramidNormals_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidNormals_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_NORMAL);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//Stop Creating Cassette
		GLES32.glBindVertexArray(0);

		//SetClearDepth
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//SetClearColor
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		bLight_rvg = false;		

		//Set Orthographic Matrix to Identity
		Matrix.setIdentityM(perspectiveProjectionMatrix_rvg, 0);
	}

	private void Resize(int width, int height)
	{
		if (height == 0)
		{
			height = 1;
		}

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix_rvg, 0,
							45.0f,
							(float)width / (float)height,
							0.1f,
							100.0f);
	}

	private void Display()
	{
		//Code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		//Use Shader Program Object
		GLES32.glUseProgram(gShaderProgramObject_rvg);

		if (bLight_rvg == true)
		{
			GLES32.glUniform1i(LKeyPressedUniform_rvg, 1);
			
			GLES32.glUniform3fv(LaUniform0_rvg, 1, light_rvg[0].lightAmbient_rvg, 0);
			GLES32.glUniform3fv(LdUniform0_rvg, 1, light_rvg[0].lightDiffuse_rvg, 0);
			GLES32.glUniform3fv(LsUniform0_rvg, 1, light_rvg[0].lightSpecular_rvg, 0);
			GLES32.glUniform4fv(lightPositionUniform0_rvg, 1, light_rvg[0].lightPosition_rvg, 0);

			GLES32.glUniform3fv(LaUniform1_rvg, 1, light_rvg[1].lightAmbient_rvg, 0);
			GLES32.glUniform3fv(LdUniform1_rvg, 1, light_rvg[1].lightDiffuse_rvg, 0);
			GLES32.glUniform3fv(LsUniform1_rvg, 1, light_rvg[1].lightSpecular_rvg, 0);
			GLES32.glUniform4fv(lightPositionUniform1_rvg, 1, light_rvg[1].lightPosition_rvg, 0);

			GLES32.glUniform3fv(KaUniform_rvg, 1, materialAmbient_rvg, 0);
			GLES32.glUniform3fv(KdUniform_rvg, 1, materialDiffuse_rvg, 0);
			GLES32.glUniform3fv(KsUniform_rvg, 1, materialSpecular_rvg, 0);

			GLES32.glUniform1f(shininessUniform_rvg, materialShininess_rvg);
		}
		else
		{
			GLES32.glUniform1i(LKeyPressedUniform_rvg, 0);
		}

		//Create Matrices
		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float rotationMatrix[] = new float[16];

		//Set to Initial Value
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);

		//Multiply Matrices
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -4.5f);
		Matrix.rotateM(rotationMatrix, 0, PAngle_rvg, 0.0f, 1.0f, 0.0f);
	
		Matrix.multiplyMM(modelMatrix, 0,
				translateMatrix, 0, 
				rotationMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix_rvg;

		GLES32.glUniformMatrix4fv(modelMatrixUniform_rvg, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform_rvg, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(perspectiveProjectionUniform_rvg, 1, false, projectionMatrix, 0);

		//Play Cassette
		GLES32.glBindVertexArray(vao_pyramid_rvg[0]);

		//Draw Object
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Stop using Shader Program Object
		GLES32.glUseProgram(0);
		requestRender();
	}

	private void Update()
	{
		//Code
		PAngle_rvg += 1.0f;
		if (PAngle_rvg >= 360.0f)
		{
			PAngle_rvg = 0.0f;	
		}
	}

	private void Uninitialize()
	{

		if (vao_pyramid_rvg[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_pyramid_rvg, 0);
			vao_pyramid_rvg[0] = 0;
		}

		if (vbo_pyramid_position_rvg[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_pyramid_position_rvg, 0);
			vbo_pyramid_position_rvg[0] = 0;
		}

		if (vbo_pyramid_normal_rvg[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_pyramid_normal_rvg, 0);
			vbo_pyramid_normal_rvg[0] = 0;
		}

		///////////////////////// Normal Shader Cleanup /////////////////////////
		//Detach Shader from Shader Program
		if(gShaderProgramObject_rvg != 0)
		{
			if (gVertexShaderObject_rvg != 0) 
			{
				GLES32.glDetachShader(gShaderProgramObject_rvg, gVertexShaderObject_rvg);
				GLES32.glDeleteShader(gVertexShaderObject_rvg);
				gVertexShaderObject_rvg = 0;
			}

			if (gFragmentShaderObject_rvg != 0)
			{
				GLES32.glDetachShader(gShaderProgramObject_rvg, gFragmentShaderObject_rvg);
				GLES32.glDeleteShader(gFragmentShaderObject_rvg);
				gFragmentShaderObject_rvg = 0;
			}
		}

		if (gShaderProgramObject_rvg != 0)
		{
			//Delete Shader Program
			GLES32.glDeleteProgram(gShaderProgramObject_rvg);
			gShaderProgramObject_rvg = 0;
		}
	}
}
