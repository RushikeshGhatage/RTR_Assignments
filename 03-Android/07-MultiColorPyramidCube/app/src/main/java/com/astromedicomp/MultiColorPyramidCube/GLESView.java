package com.astromedicomp.MultiColorPyramidCube;

import android.content.Context;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

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
	
	private int gVertexShaderObject_rvg;
	private int gFragmentShaderObject_rvg;
	private int gShaderProgramObject_rvg;
	private int mvpMatrixUniform_rvg;

	private int[] vao_pyramid_rvg = new int[1];
	private int[] vbo_pyramid_position_rvg = new int[1];
	private int[] vbo_pyramid_color_rvg = new int[1];

	private int[] vao_cube_rvg = new int[1];
	private int[] vbo_cube_position_rvg = new int[1];
	private int[] vbo_cube_color_rvg = new int[1];

	private float perspectiveProjectionMatrix_rvg[] = new float[16];	//4 X 4 Matrix

	float PAngle_rvg = 0.0f;
	float CAngle_rvg = 0.0f;

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
		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// ****************** Vertex Shader ******************
		//Create Shader
		gVertexShaderObject_rvg = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		//Provide Source Code to Shader
		final String VertexShaderCode_rvg = String.format(
														"#version 320 es" +
														"\n" +
														"in vec4 vPosition;" +
														"in vec4 vColor;" +
														"out vec4 out_color;" +
														"uniform mat4 u_mvpMatrix;" +
														"void main(void)" +
														"{"	+
															"gl_Position = u_mvpMatrix * vPosition;" +
															"out_color = vColor;" +
														"}"
													);

		//Feed to Shader Object
		GLES32.glShaderSource(gVertexShaderObject_rvg, VertexShaderCode_rvg);

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
		final String FragmentShaderCode_rvg = String.format(
														"#version 320 es" +
														"\n" +
														"precision highp float;" +
														"in vec4 out_color;" +
														"out vec4 fragColor;" +
														"void main(void)" +
														"{"	+
															"fragColor = out_color;" +
														"}"
														);

		//Feed to Shader Object
		GLES32.glShaderSource(gFragmentShaderObject_rvg, FragmentShaderCode_rvg);

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
		GLES32.glBindAttribLocation(gShaderProgramObject_rvg, GLESMacros.RVG_ATTRIBUTE_COLOR, "vColor");

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
		mvpMatrixUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_mvpMatrix");

		//Vertices Array Declaration
		final float pyramidVertices_rvg[] = new float[]
		{
			0.0f, 0.75f, 0.0f, 		//Apex
			-0.5f, -0.25f, 0.5f,	//Front Left Bottom
			0.5f, -0.25f, 0.5f, 	//Front Right Bottom

			0.0f, 0.75f, 0.0f,		//Apex
			0.5f, -0.25f, 0.5f,		//Right Left Bottom
			0.5f, -0.25f, -0.5f,	//Right Right Bottom

			0.0f, 0.75f, 0.0f,		//Apex
			0.5f, -0.25f, -0.5f,	//Back Left Bottom
			-0.5f, -0.25f, -0.5f,	//Back Right Bottom		

			0.0f, 0.75f, 0.0f,		//Apex
			-0.5f, -0.25f, -0.5f,	//Left Left Bottom
			-0.5f, -0.25f, 0.5f		//Left Right Bottom	
		};

		final float pyramidColors_rvg[] = new float[]
		{
			1.0f, 0.0f, 0.0f,	//Red 		//Front
			0.0f, 1.0f, 0.0f,	//Green
			0.0f, 0.0f, 1.0f,	//Blue

			1.0f, 0.0f, 0.0f,	//Red 		//Right
			0.0f, 0.0f, 1.0f,	//Blue
			0.0f, 1.0f, 0.0f,	//Green

			1.0f, 0.0f, 0.0f,	//Red 		//Back
			0.0f, 1.0f, 0.0f,	//Green
			0.0f, 0.0f, 1.0f,	//Blue

			1.0f, 0.0f, 0.0f,	//Red 		//Left
			0.0f, 0.0f, 1.0f,	//Blue
			0.0f, 1.0f, 0.0f	//Green
		};

		final float cubeVertices_rvg[] = new float[]
		{
			0.5f, 0.5f, 0.5f,		//Front Top Right
			-0.5f, 0.5f, 0.5f,		//Front Top Left
			-0.5f, -0.5f, 0.5f,		//Front Bottom Left
			0.5f, -0.5f, 0.5f,		//Front Bottom Right
		
			0.5f, 0.5f, -0.5f,		//Right Top Right
			0.5f, 0.5f, 0.5f,		//Right Top Left
			0.5f, -0.5f, 0.5f,		//Right Bottom Left
			0.5f, -0.5f, -0.5f,		//Right Bottom Right

			-0.5f, 0.5f, -0.5f,		//Back Top Right
			0.5f, 0.5f, -0.5f,		//Back Top Left
			0.5f, -0.5f, -0.5f,		//Back Bottom Left
			-0.5f, -0.5f, -0.5f,	//Back Bottom Right

			-0.5f, 0.5f, 0.5f,		//Left Top Right
			-0.5f, 0.5f, -0.5f,		//Left Top Left
			-0.5f, -0.5f, -0.5f,	//Left Bottom Left
			-0.5f, -0.5f, 0.5f,		//Left Bottom Right

			0.5f, 0.5f, -0.5f,		//Top Top Right
			-0.5f, 0.5f, -0.5f,		//Top Top Left
			-0.5f, 0.5f, 0.5f,		//Top Bottom Left
			0.5f, 0.5f, 0.5f,		//Top Bottom Right

			0.5f, -0.5f, 0.5f,		//Bottom Top Right
			-0.5f, -0.5f, 0.5f,		//Bottom Top Left
			-0.5f, -0.5f, -0.5f,	//Bottom Bottom Left
			0.5f, -0.5f, -0.5f		//Bottom Bottom Right
		};

		final float cubeColors_rvg[] = new float[]
		{
			1.0f, 0.0f, 0.0f,	//Red 		//FRONT
			0.0f, 1.0f, 0.0f,	//Green
			0.0f, 0.0f, 1.0f,	//Blue
			1.0f, 1.0f, 0.0f,	//Yellow

			0.0f, 0.0f, 1.0f,	//Blue 		//RIGHT
			1.0f, 0.0f, 0.0f,	//Red
			1.0f, 1.0f, 0.0f,	//Yellow
			0.0f, 1.0f, 0.0f,	//Green
		
			1.0f, 1.0f, 0.0f,	//Yellow	//BACK
			0.0f, 0.0f, 1.0f,	//Blue
			0.0f, 1.0f, 0.0f,	//Green
			1.0f, 0.0f, 0.0f,	//Red 

			0.0f, 1.0f, 0.0f,	//Green 	//LEFT
			1.0f, 1.0f, 0.0f,	//Yellow
			1.0f, 0.0f, 0.0f,	//Red
			0.0f, 0.0f, 1.0f,	//Blue

			0.0f, 0.0f, 1.0f,	//Blue 	//TOP
			1.0f, 1.0f, 0.0f,	//Yellow
			0.0f, 1.0f, 0.0f,	//Green
			1.0f, 0.0f, 0.0f,	//Red

			1.0f, 1.0f, 0.0f,	//Yellow 	//BOTTOM
			0.0f, 0.0f, 1.0f,	//Blue
			1.0f, 0.0f, 0.0f,	//Red
			0.0f, 1.0f, 0.0f,	//Green
		};

		//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao_pyramid_rvg, 0);
		GLES32.glBindVertexArray(vao_pyramid_rvg[0]);
				/////////////////////////////// FOR VERTEX ///////////////////////////////
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

				/////////////////////////////// FOR COLOR ///////////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_pyramid_color_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pyramid_color_rvg[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(pyramidColors_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(pyramidColors_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidColors_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_COLOR);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//Stop Creating Cassette
		GLES32.glBindVertexArray(0);

				//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao_cube_rvg, 0);
		GLES32.glBindVertexArray(vao_cube_rvg[0]);
				/////////////////////////////// FOR VERTEX ///////////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_cube_position_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_cube_position_rvg[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(cubeVertices_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(cubeVertices_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_POSITION);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				/////////////////////////////// FOR COLOR ///////////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_cube_color_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_cube_color_rvg[0]);

				//Fill Data into Buffer Array
				byteBuffer_rvg = ByteBuffer.allocateDirect(cubeColors_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(cubeColors_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeColors_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_COLOR);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//Stop Creating Cassette
		GLES32.glBindVertexArray(0);

		//SetClearDepth
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//GLES32.glEnable(GLES32.GL_CULL_FACE);		//Hidden Surface Removal

		//SetClearColor
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//Set Orthographic Matrix to Identity
		Matrix.setIdentityM(perspectiveProjectionMatrix_rvg, 0);
	}

	private void Resize(int width, int height)
	{
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

		//Create Matrices
		float modelViewProjectionMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float rotationMatrix[] = new float[16];

		//Translation for PYRAMID
		//Set to Initial Value
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);

		Matrix.translateM(translateMatrix, 0, -1.25f, -0.2f, -3.5f);
		Matrix.rotateM(rotationMatrix, 0, PAngle_rvg, 0.0f, 1.0f, 0.0f);

		//Multiply Matrices
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						translateMatrix, 0, 
						rotationMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						perspectiveProjectionMatrix_rvg, 0, 
						modelViewProjectionMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, false, modelViewProjectionMatrix, 0);

		//Play Cassette
		GLES32.glBindVertexArray(vao_pyramid_rvg[0]);

		//Draw Object
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Translation for CUBE
		//Set to Initial Value
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 1.25f, 0.0f, -3.5f);
		Matrix.rotateM(rotationMatrix, 0, CAngle_rvg, 0.0f, 1.0f, 0.0f);

		//Multiply Matrices
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						translateMatrix, 0, 
						rotationMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						perspectiveProjectionMatrix_rvg, 0, 
						modelViewProjectionMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, false, modelViewProjectionMatrix, 0);

		//Play Cassette
		GLES32.glBindVertexArray(vao_cube_rvg[0]);

		//Draw Object
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

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

		CAngle_rvg += 1.0f;
		if (CAngle_rvg >= 360.0f)
		{
			CAngle_rvg = 0.0f;
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