/*
 * Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
 * Distributed under the MIT License.
 * (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 */
package com.bifrostentertainment.rainbow

import android.app.NativeActivity
import android.os.Bundle
import android.view.View

//import org.fmod.FMOD

@Suppress("RedundantOverride")
class RainbowActivity : NativeActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        //FMOD.init(this)
    }

    override fun onDestroy() {
        //FMOD.close()
        super.onDestroy()
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (!hasFocus) {
            return
        }

        window.decorView.apply {
            systemUiVisibility =
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or
                    View.SYSTEM_UI_FLAG_FULLSCREEN or
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE or
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION or
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
        }
    }

    companion object {
        init {
            System.loadLibrary("c++_shared")
            //System.loadLibrary("fmod")
            //System.loadLibrary("fmodstudio")
            System.loadLibrary("rainbow")
        }
    }
}
