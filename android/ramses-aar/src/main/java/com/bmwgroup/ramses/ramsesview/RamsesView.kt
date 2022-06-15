package com.bmwgroup.ramses.ramsesview

import android.app.Application
import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.findViewTreeViewModelStoreOwner
import androidx.lifecycle.get

class RamsesView(context: Context, attrs: AttributeSet?) : SurfaceView(context, attrs), SurfaceHolder.Callback {
    companion object {
        const val TAG = "RamsesView"
    }

    private val viewModel by lazy {
        val viewModelFactory = AssetViewModelProviderFactory(context.applicationContext as Application)
        ViewModelProvider(findViewTreeViewModelStoreOwner()!!, viewModelFactory).get<AssetViewModel>()
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        viewModel.initRamses()
    }

    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
        viewModel.onSurfaceCreatedDelegate(surfaceHolder.surface)
    }

    override fun surfaceChanged(surfaceHolder: SurfaceHolder, format: Int, width: Int, height: Int) {
        viewModel.onSurfaceChangedDelegate(width, height)
    }
    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
        surfaceHolder.surface.release()
        viewModel.onSurfaceDestroyedDelegate()
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        viewModel.tearDownRamses()
    }
}