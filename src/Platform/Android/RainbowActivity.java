/**
 * Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
 * Distributed under the MIT License.
 * (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 */

package com.bifrostentertainment.rainbow;

import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import org.fmod.FMOD;

public class RainbowActivity extends NativeActivity
                          implements View.OnSystemUiVisibilityChangeListener {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("fmod");
        System.loadLibrary("fmodstudio");
        System.loadLibrary("rainbow");
    }

    // region NativeActivity overrides

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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
        FMOD.init(this);
    }

    @Override
    protected void onDestroy() {
        FMOD.close();
        super.onDestroy();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!hasFocus) {
            return;
        }
        if (!kSupportsImmersiveFullScreenMode) {
            if (kSupportsLowProfileSystemUI) {
                mDimmerHandler.removeCallbacks(mDimmerRunnable);
                dimSystemUI();
            }
            return;
        }
        contentView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    // endregion
    // region View.OnSystemUiVisibilityChangeListener implementation

    @Override
    public void onSystemUiVisibilityChange(int visibility) {
        if (BuildConfig.DEBUG && !kShouldDimSystemUI) {
            throw new RuntimeException("Not supposed to enter low profile mode.");
        }
        if ((visibility & View.SYSTEM_UI_FLAG_LOW_PROFILE) != 0) {
            return;
        }
        mDimmerHandler.removeCallbacks(mDimmerRunnable);
        mDimmerHandler.postDelayed(mDimmerRunnable, kDimmerDelay);
    }

    // endregion
    // region Private

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
        if (BuildConfig.DEBUG && !kShouldDimSystemUI) {
            throw new RuntimeException("Not supposed to enter low profile mode.");
        }
        contentView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
    }

    // endregion
}
