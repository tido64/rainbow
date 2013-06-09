/*
 * Copyright (C) 2013 Bifrost Entertainment. All rights reserved.
 */

package com.bifrostentertainment.rainbow;

import android.app.NativeActivity;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

public class RainbowActivity extends NativeActivity
                             implements View.OnSystemUiVisibilityChangeListener {
	private static final long kDimmerDelay = 5000;

	private Handler mDimmerHandler;
	private Runnable mDimmerRunnable;
	private View mContentView;

	static {
		System.loadLibrary("gnustl_shared");
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
			return;
		}
		mDimmerHandler = new Handler();
		mDimmerRunnable = new Runnable() {
			public void run() {
				dimSystemUi();
			}
		};
		mContentView = getWindow().getDecorView().findViewById(android.R.id.content);
		mContentView.setOnSystemUiVisibilityChangeListener(this);
	}

	@Override
	public void onResume() {
		super.onResume();
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
			return;
		}
		mDimmerHandler.removeCallbacks(mDimmerRunnable);
		dimSystemUi();
	}

	@Override
	public void onSystemUiVisibilityChange(int visibility) {
		if ((visibility & View.SYSTEM_UI_FLAG_LOW_PROFILE) != 0) {
			return;
		}
		mDimmerHandler.removeCallbacks(mDimmerRunnable);
		mDimmerHandler.postDelayed(mDimmerRunnable, kDimmerDelay);
	}

	private void dimSystemUi() {
		mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
	}
}
