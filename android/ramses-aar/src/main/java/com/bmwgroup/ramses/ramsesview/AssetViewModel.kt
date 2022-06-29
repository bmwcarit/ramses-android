package com.bmwgroup.ramses.ramsesview

import android.app.Application
import android.util.Log
import android.view.Surface
import androidx.lifecycle.AndroidViewModel
import com.bmwgroup.ramses.RamsesThread

open class AssetViewModel(val context: Application, val ramsesFileName: String, val rlogicFileName: String) : AndroidViewModel(context) {

    companion object {
        const val LOGGING_TAG = "AssetViewModel"
    }

    var mRamsesCallback: RamsesCallback? = null

    fun initRamses() {
        ramsesThreadImpl.initRamsesThreadAndLoadScene(context.assets, ramsesFileName, rlogicFileName)
    }

    fun tearDownRamses() {
        try {
            ramsesThreadImpl.destroyRamsesBundleAndQuitThread()
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    val ramsesThreadImpl = object : RamsesThread("RamsesThreadImpl", context) {
        override fun onUpdate() {
            mRamsesCallback?.onUpdate()
        }

        override fun onDisplayResize(width: Int, height: Int) {
            mRamsesCallback?.onDisplayResize(width, height)
        }

        override fun onSceneLoaded() {
            mRamsesCallback?.onSceneLoaded()
        }

        override fun onSceneLoadFailed() {
            mRamsesCallback?.onSceneLoadedFailed()
        }

        override fun onLogicUpdated() {
            mRamsesCallback?.onLogicUpdated()
        }
    }

    var onSurfaceCreatedDelegate = { surface: Surface ->
        try {
            ramsesThreadImpl.createDisplayAndShowScene(surface, null)
            // Setting the framerate to 30 as every frame properties are updated in onUpdate and the default of 60 would do unnecessary work
            // Setting it directly after creating the display will make sure that it will be applied for the lifetime of the display
            ramsesThreadImpl.renderingFramerate = 30.0f
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    var onSurfaceDestroyedDelegate = {
        try {
            if (ramsesThreadImpl.isAlive) {
                ramsesThreadImpl.destroyDisplay()
            }
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    var onSurfaceChangedDelegate = { width: Int, height: Int ->
        Log.i(LOGGING_TAG, "onSurfaceChangedDelegate width: $width, height: $height")
    }

}