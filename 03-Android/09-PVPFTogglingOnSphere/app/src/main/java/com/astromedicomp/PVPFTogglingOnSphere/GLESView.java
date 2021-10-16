package com.astromedicomp.PVPFTogglingOnSphere;

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
	
	///////////////////// For Per Vertex /////////////////////
	private int gVertexShaderObject_pv_rvg;
	private int gFragmentShaderObject_pv_rvg;
	private int gShaderProgramObject_pv_rvg;

	private int LaUniform_pv_rvg, LdUniform_pv_rvg, LsUniform_pv_rvg;
	private int KaUniform_pv_rvg, KdUniform_pv_rvg, KsUniform_pv_rvg;
	private int shininessUniform_pv_rvg;
	private int lightPositionUniform_pv_rvg;

	private int modelMatrixUniform_pv_rvg;
	private int viewMatrixUniform_pv_rvg;
	private int perspectiveProjectionUniform_pv_rvg;
	private int LKeyPressedUniform_pv_rvg; 

	///////////////////// For Per Fragment /////////////////////
	private int gVertexShaderObject_pf_rvg;
	private int gFragmentShaderObject_pf_rvg;
	private int gShaderProgramObject_pf_rvg;

	private int LaUniform_pf_rvg, LdUniform_pf_rvg, LsUniform_pf_rvg;
	private int KaUniform_pf_rvg, KdUniform_pf_rvg, KsUniform_pf_rvg;
	private int shininessUniform_pf_rvg;
	private int lightPositionUniform_pf_rvg;

	private int modelMatrixUniform_pf_rvg;
	private int viewMatrixUniform_pf_rvg;
	private int perspectiveProjectionUniform_pf_rvg;
	private int LKeyPressedUniform_pf_rvg; 	

	///////////////////// Common /////////////////////
	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
	private int[] vbo_sphere_normal = new int[1];
	private int[] vbo_sphere_element = new int[1];
	private int[] vbo_sphere_texture = new int [1];

	Sphere sphere = new Sphere();
	float sphere_vertices[] = new float[1146];
	float sphere_normals[] = new float[1146];
	float sphere_textures[] = new float[764];
	short sphere_elements[] = new short[2280];

	private int numVertices_rvg;
	private int numElements_rvg;

	boolean bLight_rvg = true;
	boolean bPerVertexLight_rvg = true;
	boolean bPerFragmentLight_rvg = false;

	private float[] lightAmbient_rvg = new float[] {0.0f, 0.0f, 0.0f, 1.0f};
	private float[] lightDiffuse_rvg = new float[] {1.0f, 1.0f, 1.0f, 1.0f};
	private float[] lightSpecular_rvg = new float[] {1.0f, 1.0f, 1.0f, 1.0f};
	private float[] lightPosition_rvg = new float[] {100.0f, 100.0f, 100.0f, 1.0f};

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
		if (bPerVertexLight_rvg == true)
		{
			bPerVertexLight_rvg = false;
			bPerFragmentLight_rvg = true;
		}
		else
		{
			bPerVertexLight_rvg = true;
			bPerFragmentLight_rvg = false;
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
		if (bLight_rvg == true)
		{
			bLight_rvg = false;
		}
		else
		{
			bLight_rvg = true;
		}	
		//return(true);		
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
        //////////////////////////////////////////////////////// PER VERTEX ////////////////////////////////////////////////////////
		// ****************** Vertex Shader ******************
		//Create Shader
		gVertexShaderObject_pv_rvg = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		//Provide Source Code to Shader
		final String vertexShaderCode_pv_rvg = String.format(
									"#version 320 es" +
									"\n" +
									"in vec4 vPosition_pv;" +
									"in vec3 vNormal_pv;" +
									
									"uniform vec3 u_la_pv;" +
									"uniform vec3 u_ld_pv;" +
									"uniform vec3 u_ls_pv;" +
									"uniform vec4 u_light_position_pv;" +
									
									"uniform vec3 u_ka_pv;" +
									"uniform vec3 u_kd_pv;" +
									"uniform vec3 u_ks_pv;" +
									"uniform mediump float u_materialShininess_pv;" +

									"uniform mat4 u_modelMatrix_pv;" +
									"uniform mat4 u_viewMatrix_pv;" +
									"uniform mat4 u_projectionMatrix_pv;" +

									"uniform mediump int u_lKeyPressed_pv;" +

									"out vec3 fong_ads_light_pv;" +

									"void main(void)" +
									"{"	+
										"if(u_lKeyPressed_pv == 1)" +
										"{" +
											"vec4 eye_coordinates = (u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv);" +
											"vec3 transformed_normal_pv = normalize(mat3(u_viewMatrix_pv * u_modelMatrix_pv) * vNormal_pv);" +
											"vec3 light_direction_pv = normalize(vec3(u_light_position_pv - eye_coordinates));" +
											"vec3 reflection_vector = reflect(-light_direction_pv, transformed_normal_pv);" +
											"vec3 view_vector_pv = normalize(-eye_coordinates.xyz);" +
											"vec3 ambient = u_la_pv * u_ka_pv;" +
											"vec3 diffuse = u_ld_pv * u_kd_pv * max(dot(light_direction_pv, transformed_normal_pv), 0.0);" +
											"vec3 specular = u_ls_pv * u_ks_pv * pow(max(dot(reflection_vector, view_vector_pv), 0.0), u_materialShininess_pv);" +
											"fong_ads_light_pv = ambient + diffuse + specular;" +
										"}" +
										"else" +
										"{" +
											"fong_ads_light_pv = vec3(1.0, 1.0, 1.0);" +
										"}" +
										"gl_Position = u_projectionMatrix_pv * u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv;" +
									"}");

		//Feed to Shader Object
		GLES32.glShaderSource(gVertexShaderObject_pv_rvg, vertexShaderCode_pv_rvg);

		//Compile Shader
		GLES32.glCompileShader(gVertexShaderObject_pv_rvg);

		//Shader Compilation Error Checking
		int[] infoLogLength_rvg = new int[1];
		int[] shaderCompilationStatus_rvg = new int[1];
		String szBuffer_rvg = null;
		GLES32.glGetShaderiv(gVertexShaderObject_pv_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gVertexShaderObject_pv_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gVertexShaderObject_pv_rvg);
				System.out.println("RVG: PER VERTEX Vertex Shader Compilation Log : "+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER VERTEX Vertex Shader Compilation is done Successfully.\n");
		}

		// ****************** Fragment Shader ******************
		//Create Shader
		gFragmentShaderObject_pv_rvg = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		//Provide Source Code to Shader
		final String fragmentShaderCode_pv_rvg = String.format(
														"#version 320 es" +
														"\n" +
														"precision highp float;" +
														"in vec3 fong_ads_light_pv;" +
														"out vec4 fragColor_pv;" +
														"void main(void)" +
														"{"	+
															"fragColor_pv = vec4(fong_ads_light_pv, 1.0);" +
														"}");

		//Feed to Shader Object
		GLES32.glShaderSource(gFragmentShaderObject_pv_rvg, fragmentShaderCode_pv_rvg);

		//Compile Shader
		GLES32.glCompileShader(gFragmentShaderObject_pv_rvg);	

		//Shader Compilation Error Checking
		infoLogLength_rvg[0] = 0;
		shaderCompilationStatus_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetShaderiv(gFragmentShaderObject_pv_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gFragmentShaderObject_pv_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gFragmentShaderObject_pv_rvg);
				System.out.println("RVG: PER VERTEX Fragment Shader Compilation Log :\n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER VERTEX Fragment Shader Compilation is done Successfully.\n");
		}

		//Shader Linking Code
		//1)Create the Shader Program which capable of linking Shader.
		gShaderProgramObject_pv_rvg = GLES32.glCreateProgram();

		//2)Attach whichever Shaders you have to this Shader Program Object.
		GLES32.glAttachShader(gShaderProgramObject_pv_rvg, gVertexShaderObject_pv_rvg);		//Vertex Shader
		GLES32.glAttachShader(gShaderProgramObject_pv_rvg, gFragmentShaderObject_pv_rvg);		//Fragment Shader

		//3)(Pre-Linking) Bind with Shader Program Object with Attributes
		GLES32.glBindAttribLocation(gShaderProgramObject_pv_rvg, GLESMacros.RVG_ATTRIBUTE_POSITION, "vPosition_pv");
		GLES32.glBindAttribLocation(gShaderProgramObject_pv_rvg, GLESMacros.RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

		//4)Then, Link all those attached Shader at once.
		GLES32.glLinkProgram(gShaderProgramObject_pv_rvg);

		//Shader Linkage Error Checking
		int[] shaderProgramLinkStatus_rvg = new int[1];
		infoLogLength_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetProgramiv(gShaderProgramObject_pv_rvg, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus_rvg, 0);
		if (shaderProgramLinkStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(gShaderProgramObject_pv_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetProgramInfoLog(gShaderProgramObject_pv_rvg);
				System.out.println("RVG: PER VERTEX Shader Program Link Log : \n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER VERTEX Program Shader Object Compilation is done Successfully.\n");
		}

		//Get MVP Uniform Location
		LaUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_la_pv");
		LdUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ld_pv");
		LsUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ls_pv");
		lightPositionUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_light_position_pv");

		KaUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ka_pv");
		KdUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_kd_pv");
		KsUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_ks_pv");
		shininessUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_materialShininess_pv");

		LKeyPressedUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_lKeyPressed_pv");

		modelMatrixUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_modelMatrix_pv");
		viewMatrixUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_viewMatrix_pv");
		perspectiveProjectionUniform_pv_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pv_rvg, "u_projectionMatrix_pv");


		//////////////////////////////////////////////////////// PER FRAGMENT ////////////////////////////////////////////////////////
		// ****************** Vertex Shader ******************
		//Create Shader
		gVertexShaderObject_pf_rvg = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		//Provide Source Code to Shader
		final String vertexShaderCode_pf_rvg = String.format(
									"#version 320 es" +
									"\n" +
									"in vec4 vPosition_pf;" +
									"in vec3 vNormal_pf;" +

									"out vec3 transformed_normal_pf;" +
									"out vec3 light_direction_pf;" +
									"out vec3 view_vector_pf;" +
									
									"uniform vec4 u_light_position_pf;" +
									"uniform mediump int u_lKeyPressed_pf;" +

									"uniform mat4 u_modelMatrix_pf;" +
									"uniform mat4 u_viewMatrix_pf;" +
									"uniform mat4 u_projectionMatrix_pf;" +

									"void main(void)" +
									"{"	+
										"if(u_lKeyPressed_pf == 1)" +
										"{" +
											"vec4 eye_coordinates = (u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf);" +
											"transformed_normal_pf = mat3(u_viewMatrix_pf * u_modelMatrix_pf) * vNormal_pf;" +
											"light_direction_pf = vec3(u_light_position_pf - eye_coordinates);" +
											"view_vector_pf = (-eye_coordinates.xyz);" +
										"}" +
										"gl_Position = u_projectionMatrix_pf * u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf;" +
									"}"				
									);

		//Feed to Shader Object
		GLES32.glShaderSource(gVertexShaderObject_pf_rvg, vertexShaderCode_pf_rvg);

		//Compile Shader
		GLES32.glCompileShader(gVertexShaderObject_pf_rvg);

		//Shader Compilation Error Checking
		infoLogLength_rvg = new int[1];
		shaderCompilationStatus_rvg = new int[1];
		szBuffer_rvg = null;
		GLES32.glGetShaderiv(gVertexShaderObject_pf_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gVertexShaderObject_pf_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gVertexShaderObject_pf_rvg);
				System.out.println("RVG: PER FRAGMENT Vertex Shader Compilation Log : "+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER FRAGMENT Vertex Shader Compilation is done Successfully.\n");
		}

		// ****************** Fragment Shader ******************
		//Create Shader
		gFragmentShaderObject_pf_rvg = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		//Provide Source Code to Shader
		final String fragmentShaderCode_pf_rvg = String.format(
									"#version 320 es" +
									"\n" +
									"precision highp float;" +
									"out vec4 fragColor_pf;" +

									"in vec3 transformed_normal_pf;" +
									"in vec3 light_direction_pf;" +
									"in vec3 view_vector_pf;" +
									
									"uniform vec3 u_la_pf;" +
									"uniform vec3 u_ld_pf;" +
									"uniform vec3 u_ls_pf;" +

									"uniform vec3 u_ka_pf;" +
									"uniform vec3 u_kd_pf;" +
									"uniform vec3 u_ks_pf;" +
									"uniform float u_materialShininess_pf;" +

									"uniform mediump int u_lKeyPressed_pf;" +

									"void main(void)" +
									"{"	+
									"vec3 fong_ads_light_pf;" +
									"if(u_lKeyPressed_pf == 1)" +
										"{" +
											"vec3 normalized_tranformed_normal = normalize(transformed_normal_pf);" +
											"vec3 normalized_light_direction = normalize(light_direction_pf);" +
											"vec3 normalized_view_vector = normalize(view_vector_pf);" +
											"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" +
											"vec3 ambient = u_la_pf * u_ka_pf;" +
											"vec3 diffuse = u_ld_pf * u_kd_pf * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" +
											"vec3 specular = u_ls_pf * u_ks_pf * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess_pf);" +
											"fong_ads_light_pf = ambient + diffuse + specular;" +
										"}" +
										"else" +
										"{" +
											"fong_ads_light_pf = vec3(1.0, 1.0, 1.0);" +
										"}" +
										"fragColor_pf = vec4(fong_ads_light_pf, 1.0);" +
									"}"
									);

		//Feed to Shader Object
		GLES32.glShaderSource(gFragmentShaderObject_pf_rvg, fragmentShaderCode_pf_rvg);

		//Compile Shader
		GLES32.glCompileShader(gFragmentShaderObject_pf_rvg);	

		//Shader Compilation Error Checking
		infoLogLength_rvg[0] = 0;
		shaderCompilationStatus_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetShaderiv(gFragmentShaderObject_pf_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus_rvg, 0);
		if (shaderCompilationStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gFragmentShaderObject_pf_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetShaderInfoLog(gFragmentShaderObject_pf_rvg);
				System.out.println("RVG: PER FRAGMENT Fragment Shader Compilation Log :\n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER FRAGMENT Fragment Shader Compilation is done Successfully.\n");
		}

		//Shader Linking Code
		//1)Create the Shader Program which capable of linking Shader.
		gShaderProgramObject_pf_rvg = GLES32.glCreateProgram();

		//2)Attach whichever Shaders you have to this Shader Program Object.
		GLES32.glAttachShader(gShaderProgramObject_pf_rvg, gVertexShaderObject_pf_rvg);		//Vertex Shader
		GLES32.glAttachShader(gShaderProgramObject_pf_rvg, gFragmentShaderObject_pf_rvg);		//Fragment Shader

		//3)(Pre-Linking) Bind with Shader Program Object with Attributes
		GLES32.glBindAttribLocation(gShaderProgramObject_pf_rvg, GLESMacros.RVG_ATTRIBUTE_POSITION, "vPosition_pf");
		GLES32.glBindAttribLocation(gShaderProgramObject_pf_rvg, GLESMacros.RVG_ATTRIBUTE_NORMAL, "vNormal_pf");

		//4)Then, Link all those attached Shader at once.
		GLES32.glLinkProgram(gShaderProgramObject_pf_rvg);

		//Shader Linkage Error Checking
		shaderProgramLinkStatus_rvg = new int[1];
		infoLogLength_rvg[0] = 0;
		szBuffer_rvg = null;

		GLES32.glGetProgramiv(gShaderProgramObject_pf_rvg, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus_rvg, 0);
		if (shaderProgramLinkStatus_rvg[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(gShaderProgramObject_pf_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength_rvg, 0);
			if (infoLogLength_rvg[0] > 0)
			{
				szBuffer_rvg = GLES32.glGetProgramInfoLog(gShaderProgramObject_pf_rvg);
				System.out.println("RVG: PER FRAGMENT Shader Program Link Log : \n"+szBuffer_rvg);
				Uninitialize();
				System.exit(0);
			}
		}
		else
		{
			System.out.println("RVG: PER FRAGMENT Program Shader Object Compilation is done Successfully.\n");
		}

		//Get MVP Uniform Location
		LaUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_la_pf");
		LdUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ld_pf");
		LsUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ls_pf");
		lightPositionUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_light_position_pf");

		KaUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ka_pf");
		KdUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_kd_pf");
		KsUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_ks_pf");
		shininessUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_materialShininess_pf");

		LKeyPressedUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_lKeyPressed_pf");

		modelMatrixUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_modelMatrix_pf");
		viewMatrixUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_viewMatrix_pf");
		perspectiveProjectionUniform_pf_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_pf_rvg, "u_projectionMatrix_pf");


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Vertices Array Declaration
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		numVertices_rvg = sphere.getNumberOfSphereVertices();
		numElements_rvg = sphere.getNumberOfSphereElements();

		//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);
				/////////////////////////// For POSITION ///////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_sphere_position, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

				//Fill Data into Buffer Array
				ByteBuffer byteBuffer_rvg = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(sphere_vertices);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_POSITION);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				/////////////////////////// For NORMAL ///////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(sphere_normals.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(sphere_normals);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_NORMAL);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				/////////////////////////// For ELEMENTS ///////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_sphere_element, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(sphere_elements.length * 2);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				ShortBuffer elementsBuffer_rvg = byteBuffer_rvg.asShortBuffer();
				elementsBuffer_rvg.put(sphere_elements);
				elementsBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphere_elements.length * 2, elementsBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

		//Stop Creating Cassette
		GLES32.glBindVertexArray(0);

		//SetClearDepth
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);		//Hidden Surface Removal

		bLight_rvg = false;

		//SetClearColor
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
		if (bPerVertexLight_rvg == true)
		{
			GLES32.glUseProgram(gShaderProgramObject_pv_rvg);

			if (bLight_rvg == true)
			{
				GLES32.glUniform1i(LKeyPressedUniform_pv_rvg, 1);
			
				GLES32.glUniform3fv(LaUniform_pv_rvg, 1, lightAmbient_rvg, 0);
				GLES32.glUniform3fv(LdUniform_pv_rvg, 1, lightDiffuse_rvg, 0);
				GLES32.glUniform3fv(LsUniform_pv_rvg, 1, lightSpecular_rvg, 0);

				GLES32.glUniform3fv(KaUniform_pv_rvg, 1, materialAmbient_rvg, 0);
				GLES32.glUniform3fv(KdUniform_pv_rvg, 1, materialDiffuse_rvg, 0);
				GLES32.glUniform3fv(KsUniform_pv_rvg, 1, materialSpecular_rvg, 0);

				GLES32.glUniform1f(shininessUniform_pv_rvg, materialShininess_rvg);
				GLES32.glUniform4fv(lightPositionUniform_pv_rvg, 1, lightPosition_rvg, 0);
			}
			else
			{
				GLES32.glUniform1i(LKeyPressedUniform_pv_rvg, 0);
			}
		}
		else
		{
			GLES32.glUseProgram(gShaderProgramObject_pf_rvg);
			if (bLight_rvg == true)
			{
				GLES32.glUniform1i(LKeyPressedUniform_pf_rvg, 1);
			
				GLES32.glUniform3fv(LaUniform_pf_rvg, 1, lightAmbient_rvg, 0);
				GLES32.glUniform3fv(LdUniform_pf_rvg, 1, lightDiffuse_rvg, 0);
				GLES32.glUniform3fv(LsUniform_pf_rvg, 1, lightSpecular_rvg, 0);

				GLES32.glUniform3fv(KaUniform_pf_rvg, 1, materialAmbient_rvg, 0);
				GLES32.glUniform3fv(KdUniform_pf_rvg, 1, materialDiffuse_rvg, 0);
				GLES32.glUniform3fv(KsUniform_pf_rvg, 1, materialSpecular_rvg, 0);

				GLES32.glUniform1f(shininessUniform_pf_rvg, materialShininess_rvg);
				GLES32.glUniform4fv(lightPositionUniform_pf_rvg, 1, lightPosition_rvg, 0);
			}
			else
			{
				GLES32.glUniform1i(LKeyPressedUniform_pf_rvg, 0);
			}
		}

		//Create Matrices
		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];

		//Set to Initial Value
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		//Multiply Matrices
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -2.0f);
		modelMatrix = translateMatrix;
		projectionMatrix = perspectiveProjectionMatrix_rvg;

		if (bPerVertexLight_rvg == true)
		{
			GLES32.glUniformMatrix4fv(modelMatrixUniform_pv_rvg, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniform_pv_rvg, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(perspectiveProjectionUniform_pv_rvg, 1, false, projectionMatrix, 0);
		}
		else
		{
			GLES32.glUniformMatrix4fv(modelMatrixUniform_pf_rvg, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniform_pf_rvg, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(perspectiveProjectionUniform_pf_rvg, 1, false, projectionMatrix, 0);
		}

		//Play Cassette
		GLES32.glBindVertexArray(vao_sphere[0]);

		//Draw Object
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements_rvg, GLES32.GL_UNSIGNED_SHORT, 0);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Stop using Shader Program Object
		GLES32.glUseProgram(0);
		requestRender();
	}

	private void Uninitialize()
	{

		if (vao_sphere[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
			vao_sphere[0] = 0;
		}

		if (vbo_sphere_position[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
			vbo_sphere_position[0] = 0;
		}

		if (vbo_sphere_normal[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
			vbo_sphere_normal[0] = 0;
		}

		if (vbo_sphere_element[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
			vbo_sphere_element[0] = 0;
		}

		///////////////////////// Normal Shader Cleanup /////////////////////////
		//Detach Shader from Shader Program
		if(gShaderProgramObject_pv_rvg != 0)
		{
			if (gVertexShaderObject_pv_rvg != 0) 
			{
				GLES32.glDetachShader(gShaderProgramObject_pv_rvg, gVertexShaderObject_pv_rvg);
				GLES32.glDeleteShader(gVertexShaderObject_pv_rvg);
				gVertexShaderObject_pv_rvg = 0;
			}

			if (gFragmentShaderObject_pv_rvg != 0)
			{
				GLES32.glDetachShader(gShaderProgramObject_pv_rvg, gFragmentShaderObject_pv_rvg);
				GLES32.glDeleteShader(gFragmentShaderObject_pv_rvg);
				gFragmentShaderObject_pv_rvg = 0;
			}
		}

		if (gShaderProgramObject_pv_rvg != 0)
		{
			//Delete Shader Program
			GLES32.glDeleteProgram(gShaderProgramObject_pv_rvg);
			gShaderProgramObject_pv_rvg = 0;
		}

		if(gShaderProgramObject_pf_rvg != 0)
		{
			if (gVertexShaderObject_pf_rvg != 0) 
			{
				GLES32.glDetachShader(gShaderProgramObject_pf_rvg, gVertexShaderObject_pf_rvg);
				GLES32.glDeleteShader(gVertexShaderObject_pf_rvg);
				gVertexShaderObject_pf_rvg = 0;
			}

			if (gFragmentShaderObject_pf_rvg != 0)
			{
				GLES32.glDetachShader(gShaderProgramObject_pf_rvg, gFragmentShaderObject_pf_rvg);
				GLES32.glDeleteShader(gFragmentShaderObject_pf_rvg);
				gFragmentShaderObject_pf_rvg = 0;
			}
		}

		if (gShaderProgramObject_pf_rvg != 0)
		{
			//Delete Shader Program
			GLES32.glDeleteProgram(gShaderProgramObject_pf_rvg);
			gShaderProgramObject_pf_rvg = 0;
		}
	}
}
