package com.bmwgroup.ramses.ramsesview

import android.app.Application
import android.util.Log
import android.view.Surface
import androidx.lifecycle.AndroidViewModel
import com.bmwgroup.ramses.RamsesThread

class AssetViewModel(val context: Application) : AndroidViewModel(context) {

    companion object {
        const val LOGGING_TAG = "AssetViewModel"
    }

    fun initRamses() {
        //TODO implement what is in onViewCreated
    }

    fun tearDownRamses() {
        //TODO implement what is in onDestroyView
    }


    // Overwrite these with your asset logic
    fun onUpdateDelegate() {}
    fun onSceneLoadedDelegate() {}
    fun onDisplayResizeDelegate() {}
    fun onSceneLoadedFailedDelegate() {}
    fun onLogicUpdatedDelegate() {}

    val ramsesThreadImpl = object : RamsesThread("RamsesThreadImpl", context) {
        override fun onUpdate() {
            onUpdateDelegate()
        }

        override fun onDisplayResize(width: Int, height: Int) {
            onDisplayResizeDelegate()
        }

        override fun onSceneLoaded() {
            onSceneLoadedDelegate()
        }

        override fun onSceneLoadFailed() {
            onSceneLoadedFailedDelegate()
        }

        override fun onLogicUpdated() {
            onLogicUpdatedDelegate()
        }
    }

    //TODO check if good code
    var onSurfaceCreatedDelegate = { surface: Surface ->
        try {
            ramsesThreadImpl?.createDisplayAndShowScene(surface, null)
            // Setting the framerate to 30 as every frame properties are updated in onUpdate and the default of 60 would do unnecessary work
            // Setting it directly after creating the display will make sure that it will be applied for the lifetime of the display
            ramsesThreadImpl?.renderingFramerate = 30.0f
            //onViewCreated()
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    var onSurfaceDestroyedDelegate = {
        try {
            if (ramsesThreadImpl != null && ramsesThreadImpl!!.isAlive) {
                ramsesThreadImpl?.destroyDisplay()
//                stopRendering()
//                destroyRendering()
            }
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    var onSurfaceChangedDelegate = { width: Int, height: Int ->
        Log.i(LOGGING_TAG, "onSurfaceChangedDelegate width: $width, height: $height")
    }

}