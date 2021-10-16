package com.astromedicomp.BlueScreen;

import android.content.Context;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.opengl.GLES32;	//OpenGL ES 3.2 (Version)
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;	//For OpenGL ES 1.0
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context_rvg;	//Similar to Const
	private GestureDetector gestureDetector_rvg;

	public GLESView(Context drawingContext)
	{
		super(drawingContext);

		context_rvg = drawingContext;

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

	}

	private void Resize(int width, int height)
	{
		GLES32.glViewport(0, 0, width, height);
	}

	private void Display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		requestRender();
	}

	private void Uninitialize()
	{

	}
}