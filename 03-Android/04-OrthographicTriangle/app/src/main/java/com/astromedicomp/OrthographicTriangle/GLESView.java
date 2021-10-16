package com.astromedicomp.OrthographicTriangle;

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

	private int[] vao = new int[1];
	private int[] vbo = new int[1];

	private float orthographicProjectionMatrix_rvg[] = new float[16];	//4 X 4 Matrix

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
		final String VertexShaderCode = String.format(
														"#version 320 es" +
														"\n" +
														"in vec4 vPosition;" +
														"uniform mat4 u_mvpMatrix;" +
														"void main(void)" +
														"{"	+
															"gl_Position = u_mvpMatrix * vPosition;" +
														"}"
													);

		//Feed to Shader Object
		GLES32.glShaderSource(gVertexShaderObject_rvg, VertexShaderCode);

		//Compile Shader
		GLES32.glCompileShader(gVertexShaderObject_rvg);

		//Shader Compilation Error Checking
		int[] infoLogLength = new int[1];
		int[] shaderCompilationStatus = new int[1];
		String szBuffer = null;
		GLES32.glGetShaderiv(gVertexShaderObject_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus, 0);
		if (shaderCompilationStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gVertexShaderObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(gVertexShaderObject_rvg);
				System.out.println("RVG: Vertex Shader Compilation Log : "+szBuffer);
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
		final String FragmentShaderCode = String.format(
														"#version 320 es" +
														"\n" +
														"precision highp float;" +
														"out vec4 fragColor;" +
														"void main(void)" +
														"{"	+
															"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" +
														"}"
														);

		//Feed to Shader Object
		GLES32.glShaderSource(gFragmentShaderObject_rvg, FragmentShaderCode);

		//Compile Shader
		GLES32.glCompileShader(gFragmentShaderObject_rvg);	

		//Shader Compilation Error Checking
		infoLogLength[0] = 0;
		shaderCompilationStatus[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(gFragmentShaderObject_rvg, GLES32.GL_COMPILE_STATUS, shaderCompilationStatus, 0);
		if (shaderCompilationStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gFragmentShaderObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(gFragmentShaderObject_rvg);
				System.out.println("RVG: Fragment Shader Compilation Log :\n"+szBuffer);
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

		//4)Then, Link all those attached Shader at once.
		GLES32.glLinkProgram(gShaderProgramObject_rvg);

		//Shader Linkage Error Checking
		int[] shaderProgramLinkStatus = new int[1];
		infoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(gShaderProgramObject_rvg, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus, 0);
		if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(gShaderProgramObject_rvg, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(gShaderProgramObject_rvg);
				System.out.println("RVG: Shader Program Link Log : \n"+szBuffer);
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
		final float triangleVertices[] = new float[]
		{
			0.0f, 50.0f, 0.0f, 		//Top
			-50.0f, -50.0f, 0.0f, 	//Left Bottom
			50.0f, -50.0f, 0.0f		//Right Bottom
		};

		//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

				//Fill Data into Buffer Array
				ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
				byteBuffer.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
				verticesBuffer.put(triangleVertices);
				verticesBuffer.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_POSITION);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//Stop Creating Cassette
		GLES32.glBindVertexArray(0);

		//SetClearDepth
		//GLES32.glClearDepth(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);		//Hidden Surface Removal

		//SetClearColor
		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

		//Set Orthographic Matrix to Identity
		Matrix.setIdentityM(orthographicProjectionMatrix_rvg, 0);
	}

	private void Resize(int width, int height)
	{
		GLES32.glViewport(0, 0, width, height);

		if (width <= height) 
		{
			Matrix.orthoM(orthographicProjectionMatrix_rvg, 0,
							-100.0f,
							100.0f,
							(-100.0f * (height / width)),
							(100.0f * (height / width)),
							-100.0f,
							100.0f);
		}
		else
		{
			Matrix.orthoM(orthographicProjectionMatrix_rvg, 0,
							(-100.0f * (width / height)),
							(100.0f * (width / height)),			
							-100.0f,
							100.0f,
							-100.0f,
							100.0f);	
		}
	}

	private void Display()
	{
		//Code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		//Use Shader Program Object
		GLES32.glUseProgram(gShaderProgramObject_rvg);

		//Create Matrices
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];

		//Set to Initial Value
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		//Multiply Matrices
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						orthographicProjectionMatrix_rvg, 0, 
						modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, false, modelViewProjectionMatrix, 0);

		//Play Cassette
		GLES32.glBindVertexArray(vao[0]);

		//Draw Object
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Stop using Shader Program Object
		GLES32.glUseProgram(0);
		requestRender();
	}

	private void Uninitialize()
	{

		if (vao[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
		}

		if (vbo[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo, 0);
			vbo[0] = 0;
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