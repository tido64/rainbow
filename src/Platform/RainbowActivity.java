/**
 * Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
 * Distributed under the MIT License.
 * (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 */

package com.bifrostentertainment.rainbow;

import android.app.NativeActivity;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

public class RainbowActivity extends NativeActivity
                             implements View.OnSystemUiVisibilityChangeListener {
    static {
        System.loadLibrary("gnustl_shared");
    }

    /* NativeActivity */

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (kShouldDimSystemUI) {
            mDimmerHandler = new Handler();
            mDimmerRunnable = new Runnable() {
                public void run() {
                    dimSystemUI();
                }
            };
            contentView().setOnSystemUiVisibilityChangeListener(this);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!hasFocus)
            return;
        if (!kSupportsImmersiveFullScreenMode) {
            if (kSupportsLowProfileSystemUI) {
                mDimmerHandler.removeCallbacks(mDimmerRunnable);
                dimSystemUI();
            }
            return;
        }
        contentView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    /* View.OnSystemUiVisibilityChangeListener */

    @Override
    public void onSystemUiVisibilityChange(int visibility) {
        assert kShouldDimSystemUI;
        if ((visibility & View.SYSTEM_UI_FLAG_LOW_PROFILE) != 0) {
            return;
        }
        mDimmerHandler.removeCallbacks(mDimmerRunnable);
        mDimmerHandler.postDelayed(mDimmerRunnable, kDimmerDelay);
    }

    /* Private */

    private static final boolean kSupportsImmersiveFullScreenMode =
            Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
    private static final boolean kSupportsLowProfileSystemUI =
            Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN;
    private static final boolean kShouldDimSystemUI =
            !kSupportsImmersiveFullScreenMode && kSupportsLowProfileSystemUI;
    private static final long kDimmerDelay = 5000;

    private Handler mDimmerHandler;
    private Runnable mDimmerRunnable;

    private View contentView() {
        return getWindow().getDecorView().findViewById(android.R.id.content);
    }

    private void dimSystemUI() {
        assert kShouldDimSystemUI;
        contentView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
    }
}
