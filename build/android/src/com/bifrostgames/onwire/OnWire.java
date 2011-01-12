/// The minimal amount of Java code required to get OpenGL ES up and running.
/// That is until we can start using NativeActivity class.

package com.bifrostgames.onwire;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OnWire extends Activity
{
	static
	{
		System.loadLibrary("onwire");
	}

	static public long last_frame = 0;
	static public long this_frame = 0;
	static public native void init(int width, int height);
	static public native void set_asset(String apk);
	static public native void step(long ms);

	protected OnWireView view;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		view = new OnWireView(this);
		setContentView(view);
		
		SystemClock.sleep(5000);

		// Retrieve path to APK
		try
		{
			ApplicationInfo info = getPackageManager().getApplicationInfo("com.bifrostgames.onwire", 0);
			set_asset(info.sourceDir);
		}
		catch(PackageManager.NameNotFoundException e)
		{
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
	}

	@Override
	protected void onPause()
	{
		super.onPause();
		view.onPause();
	}

	@Override
	protected void onResume()
	{
		last_frame = SystemClock.elapsedRealtime();
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
			OnWire.this_frame = SystemClock.elapsedRealtime();
			OnWire.step(OnWire.last_frame - OnWire.this_frame);
			OnWire.last_frame = OnWire.this_frame;
		}

		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			OnWire.init(width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) { }
	}
}
