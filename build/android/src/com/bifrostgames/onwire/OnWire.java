/// The minimal amount of Java code required to get OpenGL ES up and running.
/// That is until we can start using NativeActivity class.

package com.bifrostgames.onwire;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OnWire extends Activity
{
	static
	{
		System.loadLibrary("onwire");
	}

	static public native void init(int width, int height);
	static public native void step();

	protected OnWireView view;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		view = new OnWireView(this);
		setContentView(view);
	}

	@Override
	protected void onPause() {
		super.onPause();
		view.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		view.onResume();
	}
}

class OnWireView extends GLSurfaceView
{
	private static String TAG = "OnWireView";
	private static final boolean DEBUG = false;

	public Renderer renderer;

	public OnWireView(Context context)
	{
		super(context);
		renderer = new Renderer();
		setRenderer(renderer);
	}

	public boolean onTouchEvent(final MotionEvent event)
	{
		return true;
	}

	private static class Renderer implements GLSurfaceView.Renderer
	{
		public void onDrawFrame(GL10 gl)
		{
			OnWire.step();
		}

		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			OnWire.init(width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			// Do nothing.
		}
	}
}
