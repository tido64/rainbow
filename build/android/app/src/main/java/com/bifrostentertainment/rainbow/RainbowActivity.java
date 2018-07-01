/*
 * Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
 * Distributed under the MIT License.
 * (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 */

package com.bifrostentertainment.rainbow;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

//import org.fmod.FMOD;

public class RainbowActivity extends NativeActivity {

    static {
        System.loadLibrary("c++_shared");
        //System.loadLibrary("fmod");
        //System.loadLibrary("fmodstudio");
        System.loadLibrary("rainbow");
    }

    // region NativeActivity overrides

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!hasFocus) {
            return;
        }

        getContentView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //FMOD.init(this);
    }

    @Override
    protected void onDestroy() {
        //FMOD.close();
        super.onDestroy();
    }

    // endregion

    private View getContentView() {
        return getWindow().getDecorView().findViewById(android.R.id.content);
    }
}
