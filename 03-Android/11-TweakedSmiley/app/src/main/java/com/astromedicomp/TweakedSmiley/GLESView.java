package com.astromedicomp.TweakedSmiley;

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

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context_rvg;
	private GestureDetector gestureDetector_rvg;
	
	private int gVertexShaderObject_rvg;
	private int gFragmentShaderObject_rvg;
	private int gShaderProgramObject_rvg;
	private int mvpMatrixUniform_rvg;

	private int Smiley_Texture_rvg;

	private int textureSamplerUniform_rvg;

	private int[] vao_plane_rvg = new int[1];
	private int[] vbo_plane_position_rvg = new int[1];
	private int[] vbo_plane_texcoord_rvg = new int[1];
	
	private int defaultColorUniform_rvg;
	
	private int singleTap_rvg = 0;

	private float[] texCoord_rvg = new float[8];

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
		singleTap_rvg++;
		if (singleTap_rvg > 4)
		{
			singleTap_rvg = 0;
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
		// ****************** Vertex Shader ******************
		//Create Shader
		gVertexShaderObject_rvg = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		//Provide Source Code to Shader
		final String VertexShaderCode_rvg = String.format(
									"#version 320 es" +
									"\n" +
									"in vec4 vPosition;" +
									"in vec2 vTexcoord;" +
									"uniform mat4 u_mvpMatrix;" +
									"out vec2 out_Texcoord;" +
									"void main(void)" +
									"{"	+
										"gl_Position = u_mvpMatrix * vPosition;" +
										"out_Texcoord = vTexcoord;" +
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
														"in vec2 out_Texcoord;" +
														"uniform mediump int u_whiteColor;" +
														"uniform highp sampler2D u_texture_sampler;" +
														"out vec4 fragColor;" +
														"void main(void)" +
														"{"	+
															"if(u_whiteColor == 1)" +
															"{" +
																"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" +
															"}"+
															"else" +
															"{" +
																"fragColor = texture(u_texture_sampler, out_Texcoord);" +
															"}" +
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
		GLES32.glBindAttribLocation(gShaderProgramObject_rvg, GLESMacros.RVG_ATTRIBUTE_TEXCOORD, "vTexcoord");


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
		textureSamplerUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_texture_sampler");
		defaultColorUniform_rvg = GLES32.glGetUniformLocation(gShaderProgramObject_rvg, "u_whiteColor");

		//Vertices Array Declaration
		final float planeVertices_rvg[] = new float[]
		{
			1.0f, 1.0f, 0.0f,		//Front
			-1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f
		};

		//Create Cassette to use Buffer Array in Display()
		GLES32.glGenVertexArrays(1, vao_plane_rvg, 0);
		GLES32.glBindVertexArray(vao_plane_rvg[0]);
				/////////////////////////////// FOR VERTEX ///////////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_plane_position_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_plane_position_rvg[0]);

				//Fill Data into Buffer Array
				ByteBuffer byteBuffer_rvg = ByteBuffer.allocateDirect(planeVertices_rvg.length * 4);
				byteBuffer_rvg.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
				verticesBuffer_rvg.put(planeVertices_rvg);
				verticesBuffer_rvg.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, planeVertices_rvg.length * 4, verticesBuffer_rvg, GLES32.GL_STATIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_POSITION);

				//Unbind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				/////////////////////////////// FOR TEXCOORDS ///////////////////////////////
				//Generate Buffer Array
				GLES32.glGenBuffers(1, vbo_plane_texcoord_rvg, 0);
 	
				//Bind Buffer Array
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_plane_texcoord_rvg[0]);

				//Fill Data into Buffer Array
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4, null, GLES32.GL_DYNAMIC_DRAW);

				//Tell him How to Read Data from Buffer Array
				GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);

				//Enable RVG_ATTRIBUTE_POSITION Point
				GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_TEXCOORD);

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

		//Set Texture
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Smiley_Texture_rvg = loadGLTexture(R.raw.smiley);

		//Set Projection Matrix to Identity
		Matrix.setIdentityM(perspectiveProjectionMatrix_rvg, 0);
	}

	private int loadGLTexture(int imageFileResourceID)
	{
		//Local Variable Declaration
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inScaled = false;

		Bitmap bitmap = BitmapFactory.decodeResource(context_rvg.getResources(), imageFileResourceID, options);

		int texture[] = new int[1];

		//OpenGL Actual Texture Code
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

		//Setting Texture Parameter
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

		//Push Data into Graphic Memory with help of Graphic Driver
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		return(texture[0]);
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

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -4.0f);

		//Multiply Matrices
		modelViewProjectionMatrix = translateMatrix;

		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
						perspectiveProjectionMatrix_rvg, 0, 
						modelViewProjectionMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform_rvg, 1, false, modelViewProjectionMatrix, 0);

		//Set Texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, Smiley_Texture_rvg);
		GLES32.glUniform1i(textureSamplerUniform_rvg, 0);

		//Play Cassette
		GLES32.glBindVertexArray(vao_plane_rvg[0]);

		if (singleTap_rvg == 1)
		{
			texCoord_rvg[0] = 0.0f;
			texCoord_rvg[1] = 0.0f;
		
			texCoord_rvg[2] = 1.0f;
			texCoord_rvg[3] = 0.0f;
		
			texCoord_rvg[4] = 1.0f;
			texCoord_rvg[5] = 1.0f;
		
			texCoord_rvg[6] = 0.0f;
			texCoord_rvg[7] = 1.0f;
		}

		else if (singleTap_rvg == 2)
		{
			texCoord_rvg[0] = 0.5f;
			texCoord_rvg[1] = 0.5f;
			
			texCoord_rvg[2] = 1.0f;
			texCoord_rvg[3] = 0.5f;
			
			texCoord_rvg[4] = 1.0f;
			texCoord_rvg[5] = 1.0f;
			
			texCoord_rvg[6] = 0.5f;
			texCoord_rvg[7] = 1.0f;
		}

		else if (singleTap_rvg == 3)
		{
			texCoord_rvg[0] = 0.0f;
			texCoord_rvg[1] = 0.0f;
			
			texCoord_rvg[2] = 2.0f;
			texCoord_rvg[3] = 0.0f;
			
			texCoord_rvg[4] = 2.0f;
			texCoord_rvg[5] = 2.0f;
			
			texCoord_rvg[6] = 0.0f;
			texCoord_rvg[7] = 2.0f;
		}

		else if (singleTap_rvg == 4)
		{
			texCoord_rvg[0] = 0.5f;
			texCoord_rvg[1] = 0.5f;
			
			texCoord_rvg[2] = 0.5f;
			texCoord_rvg[3] = 0.5f;
		
			texCoord_rvg[4] = 0.5f;
			texCoord_rvg[5] = 0.5f;
			
			texCoord_rvg[6] = 0.5f;
			texCoord_rvg[7] = 0.5f;
		}

		if (singleTap_rvg > 0 && singleTap_rvg < 5)
		{
			GLES32.glUniform1i(defaultColorUniform_rvg, 0);
		}
		else
		{
			GLES32.glUniform1i(defaultColorUniform_rvg, 1);
		}

		//Bind Buffer Array
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_plane_texcoord_rvg[0]);

		//Fill Data into Buffer Array
		ByteBuffer byteBuffer_rvg = ByteBuffer.allocateDirect(texCoord_rvg.length * 4);
		byteBuffer_rvg.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer_rvg = byteBuffer_rvg.asFloatBuffer();
		verticesBuffer_rvg.put(texCoord_rvg);
		verticesBuffer_rvg.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (4 * 2 * 4), verticesBuffer_rvg, GLES32.GL_DYNAMIC_DRAW);

		//Tell him How to Read Data from Buffer Array
		GLES32.glVertexAttribPointer(GLESMacros.RVG_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);

		//Enable RVG_ATTRIBUTE_POSITION Point
		GLES32.glEnableVertexAttribArray(GLESMacros.RVG_ATTRIBUTE_TEXCOORD);

		//Unbind Buffer Array
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//Draw Object
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Stop Playing Cassette
		GLES32.glBindVertexArray(0);

		//Stop using Shader Program Object
		GLES32.glUseProgram(0);
		requestRender();
	}

	private void Update()
	{
		//Code

	}

	private void Uninitialize()
	{

		if (vao_plane_rvg[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_plane_rvg, 0);
			vao_plane_rvg[0] = 0;
		}

		if (vbo_plane_position_rvg[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_plane_position_rvg, 0);
			vbo_plane_position_rvg[0] = 0;
		}

		if (vbo_plane_texcoord_rvg[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_plane_texcoord_rvg, 0);
			vbo_plane_texcoord_rvg[0] = 0;
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